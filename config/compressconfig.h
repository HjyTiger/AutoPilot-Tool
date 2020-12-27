#ifndef COMPRESSCONFIG_H
#define COMPRESSCONFIG_H
#include "cf_interface.h"
#include <string>
//#include <vector>


//class TiXmlElement;


namespace DEBUGTOOL
{

using namespace std;


///
/// \brief The CCompressStrategy class
/// log file compress strategy
///
class CCompressStrategy
{
    bool            m_bPathChanged;
    std::string     m_sIssueDirName;

    explicit CCompressStrategy();


    std::string GetCurrentIssueLogDir();

    bool copy_file(const std::string& src_path, const std::string& des_path );


public:
    ~CCompressStrategy();
    CCompressStrategy(const CCompressStrategy&) = delete;
    CCompressStrategy& operator = (const CCompressStrategy&) = delete;

    static CCompressStrategy* GetInstance()
    {
        static CCompressStrategy _instance;
        return &_instance;
    }

    bool check_file_exists (const std::string& name);
    int FileSize(const char* fileName);

    bool GetPathFileName(const std::string& whole_path_name, std::string& path, std::string& file);
    bool GetFileCompressName(const std::string& file,  std::string& fileFst, std::string& cmpFile);
    bool GetFileLogName(const std::string& cmpFile, std::string& fileFst, std::string& file);

    bool CompressFile(const std::string& path,const std::string& cur_file,  const int& nCompressCpuCnt = 1, const bool& bMoved = false);
    bool MoveFile(const std::string& logPath, const std::string& fileName, const bool& bCheckSize = false);

    bool CreateDirectory(const std::string& Path);

    bool UncompressFile(const std::string& path, const std::string& fileName, std::string& uncprsFile);

    void CopyTestTagFile2IssueDir(const std::string& path);

    void SetPathChanged(const bool & b);
    bool ChangeTagFilePath(const std::string& lastPath, const std::string& curPath);
};


}

#endif
