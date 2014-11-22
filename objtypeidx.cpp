/*
    * WinDBG Anti-RootKit extension
    * Copyright � 2013-2014  Vyacheslav Rusakoff
    * 
    * This program is free software: you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation, either version 3 of the License, or
    * (at your option) any later version.
    * 
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    * 
    * You should have received a copy of the GNU General Public License
    * along with this program.  If not, see <http://www.gnu.org/licenses/>.

    * This work is licensed under the terms of the GNU GPL, version 3.  See
    * the COPYING file in the top-level directory.
*/

#include "wdbgark.h"

EXT_COMMAND(objtypeidx,
            "Output the kernel-mode ObTypeIndexTable\n",
            "")
{
    RequireKernelMode();

    Init();

    out << "******" << endlout;
    out << "*    ";
    out << std::left << std::setw( 16 ) << "Address" << std::right << std::setw( 6 ) << ' ';
    out << std::left << std::setw( 40 ) << "Object type/Routine type" << std::right << std::setw( 12 ) << ' ';
    out << std::left << std::setw( 70 ) << "Symbol" << std::right << std::setw( 4 ) << ' ';
    out << std::left << std::setw( 30 ) << "Module" << std::right << std::setw( 1 ) << ' ';
    out << "*" << endlout;
    out << "******" << endlout;

    try
    {
        if ( minor_build >= W7RTM_VER )
        {
            unsigned __int64 offset = 0;

            if ( GetSymbolOffset( "nt!ObTypeIndexTable", true, &offset ) )
            {
                for( int i = 0; i < 0x100; i++ )
                {
                    ExtRemoteData object_type_ptr( offset + i * m_PtrSize, m_PtrSize );

                    if ( i >= 3 && !object_type_ptr.GetPtr() ) // look at ObpAllocateTypeIndex()
                        break;

                    if ( object_type_ptr.GetPtr() )
                    {
                        ExtRemoteTyped object_type( "nt!_OBJECT_TYPE", object_type_ptr.GetPtr(), false, NULL, NULL );
                        DirectoryObjectTypeCallback( this, object_type, NULL );
                    }
                }
            }

        }
    }
    catch( ... )
    {
        err << "Exception in " << __FUNCTION__ << endlerr;
    }

    out << "******" << endlout;
}