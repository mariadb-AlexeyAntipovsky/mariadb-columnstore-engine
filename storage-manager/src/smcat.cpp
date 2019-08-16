/* Copyright (C) 2019 MariaDB Corporation

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA. */

#include <iostream>
#include "IOCoordinator.h"
#include <unistd.h>

using namespace std;
using namespace storagemanager;

void usage(const char *progname)
{
    cerr << progname << " is 'cat' for files managed by StorageManager." << endl;
    cerr << "Usage: " << progname << " file1 file2 ... fileN" << endl;
}

void catFile(const char *filename)
{
    uint8_t data[8192];
    off_t offset = 0;
    int read_err, write_err, count;
    IOCoordinator *ioc = IOCoordinator::get();
    
    do {
        count = 0;
        read_err = ioc->read(filename, data, offset, 8192);
        if (read_err < 0)
        {
            int l_errno = errno;
            cerr << "Error reading " << filename << ": " << strerror_r(l_errno, (char *) data, 8192) << endl;
        }

        while (count < read_err)
        {
            write_err = write(STDOUT_FILENO, &data[count], read_err - count);
            if (write_err < 0)
            {
                int l_errno = errno;
                cerr << "Error writing to stdout: " << strerror_r(l_errno, (char *) data, 8192) << endl;
                exit(1);
            }
            count += write_err;
        }
        offset += read_err;
    } while (read_err > 0);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
        catFile(argv[i]);
    
    return 0;
}