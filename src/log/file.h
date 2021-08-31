#include <filesystem>
#include <fstream>
#include <string>

namespace quark
{
  class File
  {
  public:
    File(std::string path) : m_path(path) {}

    ~File()
    {
      if (m_file.is_open())
        m_file.close();
    }

    bool open()
    {
      m_file.open(m_path,
                  std::fstream::app | std::fstream::in | std::fstream::out);
      if (!m_file.is_open())
      {
        return false;
      }

      return true;
    }

    void write(const char *buf, std::size_t size) { m_file.write(buf, size); }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

  private:
    std::fstream m_file;
    std::filesystem::path m_path;
  };
} // namespace quark