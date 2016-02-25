
#if defined( _MSC_VER )
#	ifndef _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE ( 1 )
#	endif
#	pragma warning( disable : 4996 )
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

static void comment_replacing( std::string& original )
{
	std::regex regexp_text ;

	std::cout << "Replacing single line comment...\r" ;
	regexp_text = " *//.*$" ;
	original    = std::regex_replace( original , regexp_text , "" ) ;
	std::cout << "Replace single line comment [   Finished   ]." << std::endl ;

	// not support multi comment
}

static void definition_replacing( std::string& original )
{
	std::smatch  match ;
	std::regex   regexp_text ;
	std::string  asm_search_text ;
	unsigned int count_total   = 0 ;
	unsigned int count_current = 0 ;

	regexp_text = "#define\\s+([\\w-]+)\\s+([\\w-]+)" ;

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::cout << "Counting definition (" << count_current << "/" << ++count_total << ")...\r" ;
		asm_search_text = match.suffix().str() ;
	}

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::regex  replace_regexp ;
		std::string source ;
		std::string target ;

		std::cout << "Replacing definition (" << ++count_current << "/" << count_total << ")...\r" ;

		source         = match[ 1 ] ;
		target         = match[ 2 ] ;
		replace_regexp = "\\b" + source + "\\b" ;
		original       = std::regex_replace( original , replace_regexp , target ) ;

		asm_search_text = match.suffix().str() ;
	}

	original = std::regex_replace( original , regexp_text , "" ) ;

	std::cout << "Replace definition [   Finished   ]." << std::endl ;
}

static void tritan_sharevar_replacing( std::string& original )
{
	std::smatch  match ;
	std::regex   regexp_text ;
	std::string  asm_search_text ;
	unsigned int count_total   = 0 ;
	unsigned int count_current = 0 ;

	regexp_text = "ShareVar\\s*=\\s*[{]\\s*([^}]*)[}]" ;

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::smatch sharevar_match ;
		std::regex  sharevar_regexp ;
		std::string sharevar_search_text ;

		sharevar_regexp = "(\\w+)\\s*=\\s*(\\w+)" ;

		sharevar_search_text = match[ 1 ] ;
		while( std::regex_search( sharevar_search_text , sharevar_match , sharevar_regexp ) )
		{
			std::cout << "Counting sharevar (" << count_current << "/" << ++count_total << ")...\r" ;
			sharevar_search_text = sharevar_match.suffix().str() ;
		}

		sharevar_search_text = match[ 1 ] ;
		while( std::regex_search( sharevar_search_text , sharevar_match , sharevar_regexp ) )
		{
			std::regex  replace_regexp ;
			std::string source ;
			std::string target ;

			std::cout << "Replacing sharevar (" << ++count_current << "/" << count_total << ")...\r" ;
			
			source         = sharevar_match[ 1 ] ;
			target         = sharevar_match[ 2 ] ;
			replace_regexp = "\\b" + source + "\\b" ;
			original       = std::regex_replace( original , replace_regexp , target ) ;

			sharevar_search_text = sharevar_match.suffix().str() ;
		}

		asm_search_text = match.suffix().str() ;
	}

	original = std::regex_replace( original , regexp_text , "ShareVar = {\n}" ) ;

	std::cout << "Replace sharevar [   Finished   ]." << std::endl ;
}

static void tritan_variable_replacing( std::string& original )
{
	std::smatch  match ;
	std::regex   regexp_text ;
	std::string  asm_search_text ;
	unsigned int count_total   = 0 ;
	unsigned int count_current = 0 ;

	regexp_text = "VarRM\\[\\w+:\\w+\\]\\s*=\\s*[{]\\s*([^}]*)[}]" ;

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::smatch sharevar_match ;
		std::regex  sharevar_regexp ;
		std::string sharevar_search_text ;

		sharevar_regexp = "(\\w+)\\[*.*$" ;

		sharevar_search_text = match[ 1 ] ;
		while( std::regex_search( sharevar_search_text , sharevar_match , sharevar_regexp ) )
		{
			std::cout << "Counting variable (" << count_current << "/" << ++count_total << ")...\r" ;
			sharevar_search_text = sharevar_match.suffix().str() ;
		}

		sharevar_search_text = match[ 1 ] ;
		while( std::regex_search( sharevar_search_text , sharevar_match , sharevar_regexp ) )
		{
			const unsigned int text_max_len = 1024 ;

			std::regex  replace_regexp ;
			std::string source ;
			char target[ text_max_len ] ;

			std::cout << "Replacing variable (" << ++count_current << "/" << count_total << ")...\r" ;

			sprintf_s( target , text_max_len , "__mCyb%04d" , count_current ) ;
			source         = sharevar_match[ 1 ] ;
			replace_regexp = "\\b" + source + "\\b" ;
			original       = std::regex_replace( original , replace_regexp , target ) ;

			sharevar_search_text = sharevar_match.suffix().str() ;
		}

		asm_search_text = match.suffix().str() ;
	}

	std::cout << "Replace variable [   Finished   ]." << std::endl ;
}

static void tritan_label_replacing( std::string& original )
{
	std::vector<std::string> vectPublicLabel ;
	std::smatch              match ;
	std::regex               regexp_text ;
	std::string              asm_search_text ;
	unsigned int             count_total   = 0 ;
	unsigned int             count_current = 0 ;

	regexp_text = "Public Label\\s*[{]\\s*([^}]*)[}]" ;

	std::cout << "Finding public label ...\r" ;
	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::smatch search_match ;
		std::regex  search_regexp ;
		std::string asm_find_text ;

		search_regexp = "\\b\\w+\\b" ;

		asm_find_text = match[ 1 ] ;
		while( std::regex_search( asm_find_text , search_match , search_regexp ) )
		{
			std::string text = search_match[ 0 ] ;

			vectPublicLabel.push_back( text ) ;

			asm_find_text = search_match.suffix().str() ;
		}

		asm_search_text = match.suffix().str() ;
	}
	std::cout << "Find public label [   Finished   ]." << std::endl ;

	regexp_text = "^\\s*([?]*\\w+):" ;

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		std::cout << "Counting label (" << count_current << "/" << ++count_total << ")...\r" ;
		asm_search_text = match.suffix().str() ;
	}

	asm_search_text = original ;
	while( std::regex_search( asm_search_text , match , regexp_text ) )
	{
		const unsigned int text_max_len = 1024 ;

		std::regex  replace_regexp ;
		std::string source ;
		char target[ text_max_len ] ;

		std::cout << "Replacing label (" << ++count_current << "/" << count_total << ")...\r" ;

		sprintf_s( target , text_max_len , "__lCyb%04d" , count_current ) ;
		source = match[ 1 ] ;

		bool isPublicLabel = false ;
		for( auto x : vectPublicLabel )
		{
			if( x.compare( source ) == 0 ) { isPublicLabel = true ; break ; }
		}
		if( isPublicLabel == true )
		{
			asm_search_text = match.suffix().str() ;
			continue ;
		}

		replace_regexp = "[?]" ;
		source         = std::regex_replace( source , replace_regexp , "[?]\\b" ) ;
		replace_regexp = source + "\\b" ;
		original       = std::regex_replace( original , replace_regexp , target ) ;

		asm_search_text = match.suffix().str() ;
	}

	std::cout << "Replace label [   Finished   ]." << std::endl ;
}

static void common_replacing( std::string& original )
{
	std::regex regexp_text ;

	std::cout << "Replacing tab...\r" ;
	regexp_text = "\t+" ;
	original    = std::regex_replace( original , regexp_text , "" ) ;
	std::cout << "Replace tab [   Finished   ]." << std::endl ;

	std::cout << "Replacing space...\r" ;
	regexp_text = " +$" ;
	original    = std::regex_replace( original , regexp_text , "" ) ;
	regexp_text = "  +" ;
	original    = std::regex_replace( original , regexp_text , " " ) ;
	std::cout << "Replace space [   Finished   ]." << std::endl ;

	std::cout << "Replacing empty line...\r" ;
	regexp_text = "^\\s*" ;
	original    = std::regex_replace( original , regexp_text , "" ) ;
	std::cout << "Replace empty [   Finished   ]." << std::endl ;
}

int main( int argc , char** argv )
{
	const unsigned int text_max_len = 1024 ;

	char output_file[ text_max_len ] ;

	if( argc != 2 )
	{
		std::cerr << "USAGE: TritanAsmScramble.exe <asm>\n" ;
		exit( -1 ) ;
	}

	std::ifstream source_file( argv[ 1 ] ) ;
	if( source_file.is_open() == false )
	{
		std::cerr << "TritanAsmScramble: open(" << argv[ 1 ] << ")" << "failed.\n" ;
		exit( -1 ) ;
	}
	std::string asm_text( ( std::istreambuf_iterator<char>( source_file ) ) ,
						  std::istreambuf_iterator<char>() ) ;
	source_file.close() ;

	comment_replacing( asm_text ) ;
	definition_replacing( asm_text ) ;
	tritan_sharevar_replacing( asm_text ) ;
	tritan_variable_replacing( asm_text ) ;
	tritan_label_replacing( asm_text ) ;
	common_replacing( asm_text ) ;

	strcpy_s( output_file , text_max_len , argv[ 1 ] ) ;
	for( size_t i = strlen( output_file ) - 1 ; i >= 0 ; --i )
	{
		char* p = &( output_file[ i ] ) ;

		if( *p == '.' )
		{
			strcpy_s( p , text_max_len - ( i + 1 ) , "_scrambled.ASM" ) ;
			break ;
		}

		if( i == 0 )
		{
			strcpy_s( p + 1 , text_max_len - ( i + 1 ) , "_scrambled.ASM" ) ;
			break ;
		}
	}
	
	std::ofstream scram_file( output_file ) ;
	scram_file << asm_text ;
	scram_file.close() ;

	return 0 ;
}
