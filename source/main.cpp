#include <GarrysMod/Lua/Interface.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <functional>

#include <process.hpp>

namespace process
{
	static const char meta_name[] = "Process";
	static const char invalid_handle[] = "Invalid Process handle";
	static int32_t meta_type = GarrysMod::Lua::Type::None;

	class Process : public TinyProcessLib::Process
	{
	public:
		Process( const std::vector<std::string> &arguments, const std::string &cwd, const std::unordered_map<std::string, std::string> &env ) noexcept :
			TinyProcessLib::Process(
				arguments,
				cwd,
				env,
				std::bind( &Process::ReceiveStdOut, this, std::placeholders::_1, std::placeholders::_2 ),
				std::bind( &Process::ReceiveStdErr, this, std::placeholders::_1, std::placeholders::_2 ),
				true
			)
		{ }

		Process( const std::vector<std::string> &arguments, const std::string &cwd ) noexcept :
			TinyProcessLib::Process(
				arguments,
				cwd,
				std::bind( &Process::ReceiveStdOut, this, std::placeholders::_1, std::placeholders::_2 ),
				std::bind( &Process::ReceiveStdErr, this, std::placeholders::_1, std::placeholders::_2 ),
				true
			)
		{ }

		std::vector<std::string> GetStdOut( ) noexcept
		{
			std::vector<std::string> out;

			std::lock_guard<std::mutex> lock( m_stdout_mutex );

			for( size_t k = 0; k < m_stdout_queue.size( ); ++k )
			{
				out.emplace_back( m_stdout_queue.front( ) );
				m_stdout_queue.pop( );
			}

			return out;
		}

		std::vector<std::string> GetStdErr( ) noexcept
		{
			std::vector<std::string> err;

			std::lock_guard<std::mutex> lock( m_stderr_mutex );

			for( size_t k = 0; k < m_stderr_queue.size( ); ++k )
			{
				err.emplace_back( m_stderr_queue.front( ) );
				m_stderr_queue.pop( );
			}

			return err;
		}

	private:
		void ReceiveStdOut( const char *bytes, size_t n ) noexcept
		{
			std::lock_guard<std::mutex> lock( m_stdout_mutex );
			m_stdout_queue.emplace( bytes, n );
		}

		void ReceiveStdErr( const char *bytes, size_t n ) noexcept
		{
			std::lock_guard<std::mutex> lock( m_stderr_mutex );
			m_stderr_queue.emplace( bytes, n );
		}

		std::mutex m_stdout_mutex;
		std::queue<std::string> m_stdout_queue;
		std::mutex m_stderr_mutex;
		std::queue<std::string> m_stderr_queue;
	};

	static Process *Get( GarrysMod::Lua::ILuaBase *LUA, const int32_t index )
	{
		LUA->CheckType( index, meta_type );
		Process *cmdstream = LUA->GetUserType<Process>( index, meta_type );
		if( cmdstream == nullptr )
			LUA->ArgError( index, invalid_handle );

		return cmdstream;
	}

	LUA_FUNCTION( Create )
	{
		const char *executable = LUA->CheckString( 1 );

		if( LUA->Top( ) >= 2 )
			LUA->CheckType( 2, GarrysMod::Lua::Type::Table );

		const char *cwd = nullptr;
		if( LUA->Top( ) >= 3 )
			cwd = LUA->CheckString( 3 );

		if( LUA->Top( ) >= 4 )
			LUA->CheckType( 4, GarrysMod::Lua::Type::Table );

		std::vector<std::string> args_list;
		args_list.emplace_back( executable );

		if( LUA->Top( ) >= 3 )
		{
			uint32_t num = 1;
			LUA->PushNumber( num );
			LUA->RawGet( 2 );

			const char *value = LUA->GetString( -1 );
			while( value != nullptr )
			{
				args_list.emplace_back( value );
				LUA->Pop( 1 );

				LUA->PushNumber( ++num );
				LUA->RawGet( 2 );

				value = LUA->GetString( -1 );
			}

			LUA->Pop( 1 );
		}

		bool has_env = false;
		std::unordered_map<std::string, std::string> env_list;
		if( LUA->Top( ) >= 4 )
		{
			has_env = true;

			LUA->PushNil( );
			while( LUA->Next( 4 ) != 0 )
			{
				LUA->Push( -2 );
				const char *key = LUA->GetString( -1 );
				LUA->Pop( 1 );

				const char *value = LUA->GetString( -1 );
				LUA->Pop( 1 );

				if( key != nullptr && value != nullptr )
					env_list[key] = value;
			}
		}

		std::unique_ptr<Process> cmdstream;
		if( has_env )
			cmdstream = std::make_unique<Process>( args_list, cwd != nullptr ? cwd : std::string( ), env_list );
		else
			cmdstream = std::make_unique<Process>( args_list, cwd != nullptr ? cwd : std::string( ) );

		if( cmdstream->get_id( ) <= 0 )
			return 0;

		LUA->PushUserType( cmdstream.release( ), meta_type );
		return 1;
	}

	LUA_FUNCTION( gc )
	{
		LUA->CheckType( 1, meta_type );

		Process *cmdstream = LUA->GetUserType<Process>( 1, meta_type );
		if( cmdstream == nullptr )
			return 0;

		LUA->SetUserType( 1, nullptr );
		delete cmdstream;
		return 0;
	}

	LUA_FUNCTION( index )
	{
		LUA->CheckType( 1, meta_type );

		LUA->GetMetaTable( 1 );
		LUA->Push( 2 );
		LUA->RawGet( -2 );
		if( !LUA->IsType( -1, GarrysMod::Lua::Type::NIL ) )
			return 1;

		LUA->GetFEnv( 1 );
		LUA->Push( 2 );
		LUA->RawGet( -2 );
		return 1;
	}

	LUA_FUNCTION( newindex )
	{
		LUA->CheckType( 1, meta_type );

		LUA->GetFEnv( 1 );
		LUA->Push( 2 );
		LUA->Push( 3 );
		LUA->RawSet( -3 );
		return 0;
	}

	LUA_FUNCTION( tostring )
	{
		Process *cmdstream = Get( LUA, 1 );
		LUA->PushFormattedString( "%s: %p", meta_name, cmdstream );
		return 1;
	}

	LUA_FUNCTION( eq )
	{
		Process *cmdstream1 = LUA->GetUserType<Process>( 1, meta_type );
		Process *cmdstream2 = LUA->GetUserType<Process>( 2, meta_type );
		LUA->PushBool( cmdstream1 != nullptr && cmdstream2 != nullptr && cmdstream1 == cmdstream2 );
		return 1;
	}

	LUA_FUNCTION( GetTable )
	{
		Get( LUA, 1 );
		LUA->GetFEnv( 1 );
		return 1;
	}

	LUA_FUNCTION( Write )
	{
		Process *cmdstream = Get( LUA, 1 );
		LUA->CheckType( 2, GarrysMod::Lua::Type::String );

		uint32_t size = 0;
		const char *input = LUA->GetString( 2, &size );
		LUA->PushBool( cmdstream->write( input, size ) );
		return 1;
	}

	LUA_FUNCTION( CloseInput )
	{
		Process *cmdstream = Get( LUA, 1 );
		cmdstream->close_stdin( );
		return 0;
	}

	LUA_FUNCTION( Close )
	{
		Process *cmdstream = Get( LUA, 1 );
		LUA->PushNumber( cmdstream->get_exit_status( ) );
		return 1;
	}

	LUA_FUNCTION( Kill )
	{
		Process *cmdstream = Get( LUA, 1 );

		bool force = false;
		if( LUA->Top( ) > 1 )
		{
			LUA->CheckType( 2, GarrysMod::Lua::Type::Bool );
			force = LUA->GetBool( 2 );
		}

		cmdstream->kill( force );
		return 0;
	}

	LUA_FUNCTION( GetExitCode )
	{
		Process *cmdstream = Get( LUA, 1 );

		int exit_status = 0;
		if( !cmdstream->try_get_exit_status( exit_status ) )
			return 0;

		LUA->PushNumber( exit_status );
		return 1;
	}

	LUA_FUNCTION( GetOutput )
	{
		Process *cmdstream = Get( LUA, 1 );

		const auto data = cmdstream->GetStdOut( );
		if( data.empty( ) )
			return 0;

		LUA->CreateTable( );

		for( size_t k = 0; k < data.size( ); ++k )
		{
			LUA->PushNumber( static_cast<double>( k ) + 1 );
			LUA->PushString( data[k].c_str( ), data[k].size( ) );
			LUA->SetTable( -3 );
		}

		return 1;
	}

	LUA_FUNCTION( GetErrorOutput )
	{
		Process *cmdstream = Get( LUA, 1 );

		const auto data = cmdstream->GetStdErr( );
		if( data.empty( ) )
			return 0;

		LUA->CreateTable( );

		for( size_t k = 0; k < data.size( ); ++k )
		{
			LUA->PushNumber( static_cast<double>( k ) + 1 );
			LUA->PushString( data[k].c_str( ), data[k].size( ) );
			LUA->SetTable( -3 );
		}

		return 1;
	}

	static void Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->CreateTable( );

		LUA->PushString( "process 1.0.0" );
		LUA->SetField( -2, "Version" );

		// version num follows LuaJIT style, xxyyzz
		LUA->PushNumber( 10000 );
		LUA->SetField( -2, "VersionNum" );

		LUA->PushCFunction( process::Create );
		LUA->SetField( -2, "Create" );

		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "process" );

		process::meta_type = LUA->CreateMetaTable( process::meta_name );

		LUA->PushCFunction( gc );
		LUA->SetField( -2, "__gc" );

		LUA->PushCFunction( index );
		LUA->SetField( -2, "__index" );

		LUA->PushCFunction( newindex );
		LUA->SetField( -2, "__newindex" );

		LUA->PushCFunction( tostring );
		LUA->SetField( -2, "__tostring" );

		LUA->PushCFunction( eq );
		LUA->SetField( -2, "__eq" );

		LUA->PushCFunction( GetTable );
		LUA->SetField( -2, "GetTable" );

		LUA->PushCFunction( Write );
		LUA->SetField( -2, "Write" );

		LUA->PushCFunction( CloseInput );
		LUA->SetField( -2, "CloseInput" );

		LUA->PushCFunction( Close );
		LUA->SetField( -2, "Close" );

		LUA->PushCFunction( Kill );
		LUA->SetField( -2, "Kill" );

		LUA->PushCFunction( GetExitCode );
		LUA->SetField( -2, "GetExitCode" );

		LUA->PushCFunction( GetOutput );
		LUA->SetField( -2, "GetOutput" );

		LUA->PushCFunction( GetErrorOutput );
		LUA->SetField( -2, "GetErrorOutput" );

		LUA->Pop( 1 );
	}

	static void Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->PushNil( );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "process" );
	}
}

GMOD_MODULE_OPEN( )
{
	process::Initialize( LUA );
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	process::Deinitialize( LUA );
	return 0;
}
