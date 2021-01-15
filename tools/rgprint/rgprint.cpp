//
// Created by kemm on 15.01.2021.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <utils/rowgroup/rowgroup.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <dump file>" << std::endl;
    return 0;
  }
  int fd = open(argv[1], O_RDONLY);
  struct stat st;
  fstat(fd, &st);

  messageqcpp::ByteStream bs;
  bs.needAtLeast(st.st_size);
  bs.restart();
  auto r = read(fd, bs.getInputPtr(), st.st_size);
  if (r != st.st_size)
    abort();
  bs.advanceInputPtr(r);
  rowgroup::RGData rst;
  rst.deserialize(bs);

  std::vector<uint32_t> pos{2, 6, 22, 30, 46, 54}; // ?
  std::vector<uint32_t> oids{3011, 3011, 3011, 3011, 3011}; // ?
  std::vector<uint32_t> keys{1, 1, 1, 1, 1}; // ?
  std::vector<execplan::CalpontSystemCatalog::ColDataType> col_t{
      execplan::CalpontSystemCatalog::INT,
      execplan::CalpontSystemCatalog::LONGDOUBLE,
      execplan::CalpontSystemCatalog::UBIGINT,
      execplan::CalpontSystemCatalog::LONGDOUBLE,
      execplan::CalpontSystemCatalog::UBIGINT
  };
  std::vector<uint32_t> csN{8, 8, 8, 8, 8};
  std::vector<uint32_t> scale{0, 0, 0, 0, 0};
  std::vector<uint32_t> prec{10, 4294967295, 9999, 4294967295, 19};
  rowgroup::RowGroup rg(5, pos, oids, keys, col_t, csN, scale, prec, 20, false, std::vector<bool>{});
  rg.setData(&rst);
  close(fd);
  std::cout << "RowGroup data:\n" << rg.toString() << std::endl;
  return 0;
}