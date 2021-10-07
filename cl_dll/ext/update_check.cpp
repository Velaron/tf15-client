#ifdef CURRENT_VERSION

#include "update_check.h"

#include "ext/IGameMenuExports.h"
extern IGameMenuExports *g_pMainUI;

#if defined( _WIN32 )
#include <windows.h>
#include <winhttp.h>

void CheckForUpdates( void )
{
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0, dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL, pszCommitSha = NULL;
	char szCommitSha[8] = "";

	hSession = WinHttpOpen( L"tf15-client", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0 );

	if ( hSession )
	{
		if ( !WinHttpSetTimeouts( hSession, 1000, 1000, 1000, 1000 ) )
		{
			WinHttpCloseHandle( hSession );
			return;
		}

		hConnect = WinHttpConnect( hSession, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0 );
	}

	if ( hConnect )
		hRequest = WinHttpOpenRequest( hConnect, NULL, L"/repos/Velaron/tf15-client/commits/master?per_page=1", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE );

	if ( hRequest )
		bResults = WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0 );

	if ( bResults )
		bResults = WinHttpReceiveResponse( hRequest, NULL );

	for ( ;; )
	{
		dwSize = 0;

		if ( !WinHttpQueryDataAvailable( hRequest, &dwSize ) )
			break;

		if ( !dwSize )
			break;

		pszOutBuffer = (LPSTR)malloc( dwSize + 1 );

		if ( !pszOutBuffer )
			break;

		ZeroMemory( pszOutBuffer, dwSize + 1 );

		if ( WinHttpReadData( hRequest, pszOutBuffer, dwSize, &dwDownloaded ) )
		{
			char *pch = strstr( pszOutBuffer, "sha" );

			if ( pch )
			{
				pch += 6;                                               // skip to the commit hash
				strncpy( szCommitSha, pch, sizeof( szCommitSha ) - 1 ); // copy the commit hash to a buffer
				szCommitSha[7] = '\0';                                  // null terminate

				free( pszOutBuffer );
				break;
			}
		}

		free( pszOutBuffer );

		if ( !dwDownloaded )
			break;
	}

	if ( hSession )
		WinHttpCloseHandle( hSession );
	if ( hConnect )
		WinHttpCloseHandle( hConnect );
	if ( hRequest )
		WinHttpCloseHandle( hRequest );

	if ( strcmp( szCommitSha, CURRENT_VERSION ) )
		g_pMainUI->ShowUpdateDialog();
}
#elif defined( __linux__ ) && !defined( __ANDROID__ )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

typedef struct memory_s
{
	char *data;
	size_t size;
} memory_t;

size_t WriteCallback( char *ptr, size_t size, size_t nmemb, void *userdata )
{
	size_t realsize = size * nmemb;
	memory_t *mem = (memory_t *)userdata;

	char *p = (char *)realloc( mem->data, mem->size + realsize + 1 );

	if ( !p )
		return 0;

	mem->data = p;
	memcpy( &mem->data[mem->size], ptr, realsize );
	mem->size += realsize;
	mem->data[mem->size] = 0;

	return realsize;
}

void CheckForUpdates( void )
{
	CURL *curl;
	CURLcode res;
	memory_t response;
	char szCommitSha[8] = "";

	response.data = (char *)malloc( 1 );
	response.size = 0;

	curl_global_init( CURL_GLOBAL_DEFAULT );

	curl = curl_easy_init();

	if ( curl )
	{
		curl_easy_setopt( curl, CURLOPT_URL, "https://api.github.com/repos/Velaron/tf15-client/commits/master?per_page=1" );
		curl_easy_setopt( curl, CURLOPT_USERAGENT, "tf15-client" );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)&response );

		res = curl_easy_perform( curl );

		if ( res == CURLE_OK )
		{
			char *pch = strstr( response.data, "sha" );

			if ( pch )
			{
				pch += 6;
				strncpy( szCommitSha, pch, sizeof( szCommitSha ) - 1 );
				szCommitSha[7] = '\0';
			}
		}

		curl_easy_cleanup( curl );
	}

	free( response.data );
	curl_global_cleanup();

	if ( strcmp( szCommitSha, CURRENT_VERSION ) )
		g_pMainUI->ShowUpdateDialog();
}
#else
void CheckForUpdates( void )
{
}
#endif

#else
void CheckForUpdates( void )
{
}
#endif // CURRENT_VERSION