/*----------------------------------------------------------------------------
	Program : fminmax/GetInput.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get user input from the form.

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


#include	"fminmax.h"


void GetInput ()
{
	int		xa;

	if ( webGetInput () != 0 )
	{
		printf ( "webGetInput: Input error!<br>\n" );
	}

	for ( xa = 0; xa < webCount; xa++ )
	{
		webFixHex ( webValues[xa] );

#ifdef DEBUG
		printf ( "GetInput: %s = %s<br>", webNames[xa] , webValues[xa] );
#endif

		if ( nsStrcmp ( webNames[xa], "MinMax" ) == 0 )
		{
			FieldName = webValues[xa];
		}
		else if ( nsStrcmp ( webNames[xa], "Type" ) == 0 )
		{
			Source = webValues[xa][0];
		}
		else
		{
			printf ( "GetInput: UNKNOWN NAME/VALUE %s = %s<br>", webNames[xa] , webValues[xa] );
		}

	}
}
