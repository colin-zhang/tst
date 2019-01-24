#include <stdio.h>
#include <stdlib.h>
#include "hiredis/hiredis.h"

void ProcessReply( redisReply * pReply ) {
    redisReply * pSubReply = NULL;

    if ( pReply != NULL && pReply->elements == 3 ) {
        pSubReply = pReply->element[2];
        printf( "Msg [%s]\n", pSubReply->str );
    }
}

int main( int argc, char const *argv[] ) {
    redisContext * pContext = redisConnect( "127.0.0.1", 6379 );

    if ( NULL == pContext || pContext->err == 1 ) {
        printf( "%s\n", pContext->errstr );
        exit( -1 );
    }

    const char* pKey = "DATABUS:REQ";

    redisReply * pReply = (redisReply*)redisCommand( pContext, "SUBSCRIBE %s", pKey);
    freeReplyObject( pReply );
    while ( redisGetReply( pContext, (void **)&pReply ) == REDIS_OK ) {
        ProcessReply( pReply );
        freeReplyObject( pReply );
    }

    redisFree( pContext );

    return 0;
}

