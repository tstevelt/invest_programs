/*----------------------------------------------------------------------------
	Program : wlupld/wlupld.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Called by website Import Watchlist screen.

	Who		Date		Modification
	---------------------------------------------------------------------

----------------------------------------------------------------------------*/
//     Programs called by invest.cgi
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#define		MAIN
#include	"wlupld.h"

int		UseErrorSys = 1;

int main ( int argc, char *argv[] )
{
	int		ByteCount;
	char	tempfn[80];
	FILE	*tempfp;
	int		Char;
	WEBPARMS    *ptrWebParms;

	ptrWebParms = webInitParms ();

	webContentHtml ();
        
    ptrWebParms->WP_Title = "Import Watchlist";
    webStartHead ( ptrWebParms );

	ptrWebParms->WP_Color = STORY_BACKGROUND;
//	ptrWebParms->WP_OnLoad = "javascript:window.opener.reload();";
	webHeadToBody ( ptrWebParms );


	ByteCount = 0;
	sprintf ( tempfn, "/var/local/tmp/wpupld.txt" );
	if (( tempfp = fopen ( tempfn, "w" )) == (FILE *) 0 )
	{
		printf ( "can not create temp file [%s] for storage<br>\n", tempfn );
	}
	else
	{
		while (( Char = fgetc ( stdin )) != EOF )
		{
			fputc ( Char, tempfp );
			ByteCount++;
		}
		nsFclose ( tempfp );
	}

	if ( ByteCount == 0 )
	{
		printf ( "Nothing uploaded<br>\n" );
		exit ( 0 );
	}

	StartMySQL ( &MySql, "invest" );
	DoFile ( tempfn );

	unlink ( tempfn );

	printf ( "</body>\n" );
	printf ( "</html>\n" );

	webFreeParms ( ptrWebParms );

	return ( 0 );
}
