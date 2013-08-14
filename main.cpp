#include <GarrysMod/Lua/Interface.h>
#include <exec-stream.h>
#include <sstream>

#define PROCESS_META_NAME "Process"
#define PROCESS_META_ID 254

struct ExtendedUserData
{
	void *data;
	unsigned char type;
	int ref;
};

LUA_FUNCTION( Process_New )
{
	try
	{
		exec_stream_t *cmdstream = new exec_stream_t( );

		ExtendedUserData *userdata = (ExtendedUserData *)LUA->NewUserdata( sizeof( ExtendedUserData ) );
		userdata->type = PROCESS_META_ID;
		userdata->data = cmdstream;

		LUA->CreateTable( );
		userdata->ref = LUA->ReferenceCreate( );

		LUA->CreateMetaTableType( PROCESS_META_NAME, PROCESS_META_ID );
		LUA->SetMetaTable( -2 );

		return 1;
	}
	catch( const std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_Delete )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		return 0;
	}

	LUA->ReferenceFree( userdata->ref );
	delete (exec_stream_t *)userdata->data;

	return 0;
}

LUA_FUNCTION( Process_Index )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	LUA->GetMetaTable( 1 );
	LUA->PushString( "__functions" );
	LUA->GetTable( -2 );

	LUA->Push( 2 );
	LUA->GetTable( -2 );

	if( LUA->GetType( -1 ) != GarrysMod::Lua::Type::NIL )
	{
		LUA->Remove( -3 ); // Metatable is removed here.
		LUA->Remove( -2 ); // __functions is removed here.
		return 1;
	}

	LUA->Pop( 3 ); // Object (nil), __functions and metatable are removed here.

	if( userdata->ref == -2 )
	{
		return 0;
	}

	LUA->ReferencePush( userdata->ref );
	LUA->Push( 2 );
	LUA->GetTable( -2 );
	LUA->Remove( -2 ); // Value pushed from reference removed here.
	return 1;
}

LUA_FUNCTION( Process_NewIndex )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	if( userdata->ref == -2 )
	{
		LUA->CreateTable( );
		userdata->ref = LUA->ReferenceCreate( );
	}

	LUA->ReferencePush( userdata->ref );
	LUA->Push( 2 );
	LUA->Push( 3 );
	LUA->SetTable( -3 );

	LUA->Pop( );

	return 0;
}

LUA_FUNCTION( Process_ToString )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	std::stringstream tostring;
	tostring << PROCESS_META_NAME ": " << userdata->data;
	LUA->PushString( tostring.str( ).c_str( ) );
	return 1;
}

LUA_FUNCTION( Process_Equal )
{
	LUA->CheckType( 1, PROCESS_META_ID );
	LUA->CheckType( 2, PROCESS_META_ID );

	ExtendedUserData *userdata1 = (ExtendedUserData *)LUA->GetUserdata( 1 );
	ExtendedUserData *userdata2 = (ExtendedUserData *)LUA->GetUserdata( 2 );

	LUA->PushBool( userdata1->data == userdata2->data );
	return 1;
}

LUA_FUNCTION( Process_Start )
{
	LUA->CheckType( 1, PROCESS_META_ID );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;

		std::vector<std::string> args_list;

		if( LUA->IsType( 3, GarrysMod::Lua::Type::TABLE ) )
		{
			unsigned int num = 1;
			LUA->PushNumber( (double)num );
			LUA->RawGet( 3 );
			while( LUA->IsType( -1, GarrysMod::Lua::Type::STRING ) )
			{
				args_list.push_back( LUA->GetString( -1 ) );
				LUA->Pop( 1 );

				LUA->PushNumber( (double)++num );
				LUA->RawGet( 3 );
			}
		}
		else if( LUA->IsType( 3, GarrysMod::Lua::Type::STRING ) )
		{
			args_list.push_back( LUA->GetString( 3 ) );
		}

		cmdstream->start( LUA->GetString( 2 ), args_list.begin( ), args_list.end( ) );

		LUA->PushBool( true );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_SetTimeout )
{
	LUA->CheckType( 1, PROCESS_META_ID );
	LUA->CheckType( 2, GarrysMod::Lua::Type::NUMBER );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;
		cmdstream->set_wait_timeout( exec_stream_t::s_child, (exec_stream_t::timeout_t)LUA->GetNumber( 2 ) );
		LUA->PushBool( true );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_CloseInput )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;
		LUA->PushBool( cmdstream->close_in( ) );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_Close )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;
		LUA->PushBool( cmdstream->close( ) );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_Kill )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;
		cmdstream->kill( );
		LUA->PushBool( true );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_GetExitCode )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;
		LUA->PushNumber( (double)cmdstream->exit_code( ) );
		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_GetOutput )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;

		LUA->CreateTable( );

		unsigned int num = 0;
		std::string out;
		while( std::getline( cmdstream->out( ), out ).good( ) )
		{
			LUA->PushNumber( (double)++num );
			LUA->PushString( out.c_str( ), out.size( ) );
			LUA->SetTable( -3 );
		}

		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

LUA_FUNCTION( Process_GetErrorOutput )
{
	LUA->CheckType( 1, PROCESS_META_ID );

	ExtendedUserData *userdata = (ExtendedUserData *)LUA->GetUserdata( 1 );
	if( !userdata->data )
	{
		LUA->ArgError( 1, "Invalid " PROCESS_META_NAME " handle" );
		return 0;
	}

	try
	{
		exec_stream_t *cmdstream = (exec_stream_t *)userdata->data;

		LUA->CreateTable( );

		unsigned int num = 0;
		std::string out;
		while( std::getline( cmdstream->err( ), out ).good( ) )
		{
			LUA->PushNumber( (double)++num );
			LUA->PushString( out.c_str( ), out.size( ) );
			LUA->SetTable( -3 );
		}

		return 1;
	}
	catch( std::exception &e )
	{
		LUA->PushNil( );
		LUA->PushString( e.what( ) );
		return 2;
	}
}

GMOD_MODULE_OPEN( )
{
	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );

	LUA->PushCFunction( Process_New );
	LUA->SetField( -2, "Process" );

	LUA->Pop( 1 );

	LUA->CreateMetaTableType( PROCESS_META_NAME, PROCESS_META_ID );

	LUA->PushCFunction( Process_Delete );
	LUA->SetField( -2, "__gc" );

	LUA->PushCFunction( Process_Index );
	LUA->SetField( -2, "__index" );

	LUA->PushCFunction( Process_NewIndex );
	LUA->SetField( -2, "__newindex" );

	LUA->PushCFunction( Process_ToString );
	LUA->SetField( -2, "__tostring" );

	LUA->PushCFunction( Process_Equal );
	LUA->SetField( -2, "__eq" );

	LUA->CreateTable( );

	LUA->PushCFunction( Process_Start );
	LUA->SetField( -2, "Start" );

	LUA->PushCFunction( Process_SetTimeout );
	LUA->SetField( -2, "SetTimeout" );

	LUA->PushCFunction( Process_CloseInput );
	LUA->SetField( -2, "CloseInput" );

	LUA->PushCFunction( Process_Close );
	LUA->SetField( -2, "Close" );

	LUA->PushCFunction( Process_Kill );
	LUA->SetField( -2, "Kill" );

	LUA->PushCFunction( Process_GetExitCode );
	LUA->SetField( -2, "GetExitCode" );

	LUA->PushCFunction( Process_GetOutput );
	LUA->SetField( -2, "GetOutput" );

	LUA->PushCFunction( Process_GetErrorOutput );
	LUA->SetField( -2, "GetErrorOutput" );

	LUA->SetField( -2, "__functions" );

	LUA->Pop( 1 );

	return 0;
}

GMOD_MODULE_CLOSE( )
{
	return 0;
}