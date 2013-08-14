#include <exec-stream.h>

int main( int argc, char **argv )
{
	try
	{
		exec_stream_t process;

		process.start( "tasklist", "" );

		std::string out;
		while( std::getline( process.out( ), out ).good( ) )
		{
			printf( "%s\n", out.c_str( ) );
		}

		process.close( );
	}
	catch( const std::exception &e )
	{
		printf( "%s\n", e.what( ) );
		return -1;
	}

	return 0;
}
