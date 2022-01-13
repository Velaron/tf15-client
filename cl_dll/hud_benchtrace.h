#if !defined( HUD_BENCHTRACE_H )
#define HUD_BENCHTRACE_H
#pragma once

void Trace_StartTrace( int *results, int *finished, const char *pszServer );
void Trace_Think( void );

#endif // HUD_BENCHTRACE_H