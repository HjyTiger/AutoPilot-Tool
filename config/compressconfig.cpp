#include "compressconfig.h"

#include <iostream>
#include <fstream>
//#include <algorithm>
//#include <dirent.h>
//#include <iomanip>

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global.hpp"


using namespace DEBUGTOOL;


///
/// \brief CCompressStrategy::CCompressStrategy
/// compress method
///
CCompressStrategy::CCompressStrategy()
    : m_sIssueDirName("")
    , m_bPathChanged(false)
{

}

CCompressStrategy::~CCompressStrategy()
{

}


///
/// \brief CCompressStrategy::GetPathFileName
/// \param whole_path_name
/// \param path
/// \param cmpFile
/// \return
/// check file and path
///
bool CCompressStrategy::GetPathFileName(const std::string& logFile, std::string& path, std::string& file)
{
    const int nPathFileLen = logFile.length();
    const int nPathIndex = logFile.find_last_of("/");
    if( 0 >= nPathIndex)
    {
        return false;
    }

    path = logFile.substr(0,nPathIndex);
    file = logFile.substr(nPathIndex+1, nPathFileLen - nPathIndex );

    return true;
}


///
/// \brief CCompressStrategy::GetFileCompressName
/// \param file
/// \param fileFst
/// \param cmpFile
/// \return
/// get filename and its suf
///
bool CCompressStrategy::GetFileCompressName(const std::string& file, std::string& fileFst, std::string& cmpFile)
{
    const int nFileNameIndex = file.find_first_of(".");
    if( 0 >=  nFileNameIndex)
    {
        return false;
    }

    fileFst = file.substr(0,nFileNameIndex);
    cmpFile = fileFst + DEBUGTOOL::COMPRESS_SUFFIX;

    return true;
}

///
/// \brief CCompressStrategy::GetFileLogName
/// \param file
/// \param fileFst
/// \param cmpFile
/// \return
///  get file name from compressed name
///
bool CCompressStrategy::GetFileLogName(const std::string& cmpFile, std::string& fileFst, std::string& file)
{
    const int nFileNameIndex = cmpFile.find_first_of(".");
    if( 0 >=  nFileNameIndex)
    {
        return false;
    }

    fileFst = cmpFile.substr(0,nFileNameIndex);
    file = fileFst + DEBUGTOOL::LOG_SUFFIX;

    return true;
}

///
/// \brief CCompressStrategy::CompressFile
/// \param cur_file
/// \param cmpFile
/// \return
/// compress log file
///
bool CCompressStrategy::CompressFile(const std::string& path_def, const std::string& logName_def, const int& nCompressCpuCnt, const bool& bMoved)
{
    std::string path = path_def;
    if(path.empty())
    {
        return false;
    }

    std::string fileFst="";
    std::string logName = logName_def;

    if(bMoved)
    {
        path += "/" + GetCurrentIssueLogDir();
    }

    std::string cmpFile="";

    if(!GetFileCompressName(logName, fileFst, cmpFile))
    {
        return false;
    }

//    QString path = logFile.left(logFile.lastIndexOf("/"));
//    QString file = logFile.right(logFile.length()-logFile.lastIndexOf("/")-1);
//    QString fileFst = file.left(file.lastIndexOf("."));
//    QString cmpFile = fileFst + DEBUGTOOL::COMPRESS_SUFFIX;
//    QString cmd = "cd \"" + path + "\"; tar -cvf " + fileFst + ".tar " + file  + "; xz -z " + fileFst + ".tar";

    static std::string lastPath = "";
    if(path != lastPath)
    {
        //check dir
        if(!CreateDirectory(path))
        {
            return false;
        }
        lastPath = path;
    }

    std::string cmd = "";
    int nRes = 1;

    if(!check_file_exists(path + "/" + logName))
    {
        return false;
    }

    if(check_file_exists(path + "/" + cmpFile))
    {
        return true;
    }

    const std::string delCmd = "if [ $? -eq 0 ];then rm -rf " + logName + "; fi";

    if(DEBUGTOOL::COMPRESS_SUFFIX == ".tar.xz")
    {
        if ( 1 == nCompressCpuCnt)
        {
            cmd = "cd \"" + path + "\"; tar -cJf " + cmpFile + " " + logName + "; " + delCmd;
        }
        else
        {
            const std::string fileTmp = fileFst + ".tar";
//            cmd = "cd \"" + path + "\"; tar -cvf " + fileTmp + " " + logName + "; xz -T " + std::to_string(nCompressCpuCnt) + " -z " + fileTmp +";" + delCmd;
            cmd = "cd \"" + path + "\"; tar -cvf " + fileTmp + " " + logName + "; xz -T 2 -z " + fileTmp +";" + delCmd;
        }
    }
    else if(DEBUGTOOL::COMPRESS_SUFFIX == ".tar.gz")
    {
        cmd = "cd \"" + path + "\"; tar -czf " + cmpFile + " " + logName + "; " + delCmd;
    }
    else
    {
        return false;
    }
    nRes = system(cmd.c_str());

    return (0 == nRes);

//    {
//        CheckMoveFile(cmpFile);
//    }

    // compress
    //tar -cvf xxx.tar xxx
    //xz -z xxx.tar
    //or:
    //tar -cvJf xxx.tar.xz xxx

    // uncompress
//    tar -xvjf xxx.tar.xz
}

///
/// \brief CCompressStrategy::FileSize
/// \param fileName
/// \return
///
int CCompressStrategy::FileSize(const char* fileName)
{
    struct stat statbuf;
    if(stat(fileName,&statbuf) == 0)
    {
        return statbuf.st_size;
    }
    return -1;
}

//#include <sys/stat.h>

bool CCompressStrategy::check_file_exists (const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


///
/// \brief CCompressStrategy::GetCurrentIssueDir
/// \return
/// return issue_log_%Y%m%d%H%M%S, only once for a running tool
///
std::string CCompressStrategy::GetCurrentIssueLogDir()
{
    if(!m_sIssueDirName.empty())
    {
        return m_sIssueDirName;
    }

    const std::string issDir = DEBUGTOOL::LOG_ISSUE_SUBDIR;//"issue_log";
    const std::string sDateTime = GetCurrentDateTimeString_3();

    m_sIssueDirName = issDir + "_" + sDateTime;

    return m_sIssueDirName;
}

///
/// \brief CCompressStrategy::MoveSingleFile
/// \param logPath
/// \param fileName
/// \param bCheckSize
/// \return
/// move issue file to issue directory
///
bool CCompressStrategy::MoveFile(const std::string& logPath, const std::string& fileName, const bool& bCheckSize)
{
    if(logPath.empty())
    {
        return false;
    }

    const std::string issDir = GetCurrentIssueLogDir();
    const std::string issPath =  logPath + "/" + issDir;

    static bool bCreateIssueDir = false;

    if(m_bPathChanged)
    {
        bCreateIssueDir = false;
        m_bPathChanged = false;
    }
    if(! bCreateIssueDir)
    {

        if(! CreateDirectory(issPath))
        {
            return false;
        }
        else
        {
             bCreateIssueDir = true;
        }
//        const std::string cmd = "if [  ! -d  " + issPath + "  ];then mkdir  -p " + issPath + " ; fi;";

//        const int nRes = system(cmd.c_str());

//        if(0 != nRes)
//        {
//            //create issue sub-dir error
//            return false;
//        }
//        else
//        {
//            bCreateIssueDir = true;
//        }
    }

    if(bCheckSize)
    {
        // check file size, if < 0 return, else move file
        const std::string logFilePath = logPath + fileName;
        for(int i = 0; i< 3;  i++)
        {
            if(0 < FileSize(logFilePath.c_str()))
            {
                break;
            }
            else
            {
                if(i == 2 )
                {
                    return false;
                }
                usleep(10000);
            }
        }
    }

//    int nSufLen = strlen(DEBUGTOOL::COMPRESS_SUFFIX);
//    std::string fileMainName = fileName.substr(0,fileName.size()-nSufLen);
//    std::string fileLog = fileMainName + DEBUGTOOL::LOG_SUFFIX;

    const std::string cmd = "cd \"" + logPath + "\" ;mv " + fileName + " ./" + issDir + "; ";
    const int nRes = system(cmd.c_str());

    return (0 == nRes);
}


void CCompressStrategy::SetPathChanged(const bool & bChanged)
{
    m_bPathChanged = true;
}

///
/// \brief CCompressStrategy::CreateDirectory
/// \param logPath
/// \return
/// check and create dir
///
bool CCompressStrategy::CreateDirectory(const std::string& path)
{    
    if(0 == access(path.c_str(),0))
    {
        return true;
    }
    int nres = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);


    bool bCheck = (0 == nres);

    return bCheck;
}

///
/// \brief CCompressStrategy::UncompressFile
/// \param path
/// \param fileName
/// \param uncprsFile
/// \return
/// check and uncompress
///
bool CCompressStrategy::UncompressFile(const std::string& path, const std::string& cmpFile, std::string& uncprsFile)
{
    int nLogSufLen = strlen(DEBUGTOOL::LOG_SUFFIX);
    int nFileLen = cmpFile.length();
    if(nFileLen <= nLogSufLen)
    {
        return false;
    }

    int nFileNameIndex = cmpFile.find_first_of(".");
    if( 0 >=  nFileNameIndex)
    {
        return false;
    }

    std::string cur_suf = cmpFile.substr(nFileNameIndex,nFileLen - nFileNameIndex);

    // check log file is cmpFile
    if(0 == cur_suf.compare(DEBUGTOOL::LOG_SUFFIX))
    {
        uncprsFile = cmpFile;
        return true;
    }

    std::string fileFst = cmpFile.substr(0,nFileNameIndex);
    uncprsFile = fileFst + DEBUGTOOL::LOG_SUFFIX;

    std::string cmd = "";

    // check original log file exist
    std::string logPathFile = path + "/" + uncprsFile;
    if(0 < FileSize(logPathFile.c_str()))
    {
        return true;
    }

    // uncompress file
    if(DEBUGTOOL::COMPRESS_SUFFIX == ".tar.xz")
    {
        cmd = "cd \"" + path + "\"; tar -xJf " + cmpFile + "; ";
    }
    else if(DEBUGTOOL::COMPRESS_SUFFIX == ".tar.gz")
    {
        cmd = "cd \"" + path + "\"; tar -xzf " + cmpFile + "; ";
    }
    else
    {
        return false;
    }

    int nRes = system(cmd.c_str());

    bool bFileSize = (FileSize(logPathFile.c_str()) > 0 );

    return (0 == nRes) && bFileSize;
}

///
/// \brief CCompressStrategy::ChangeTagFilePath
/// \param lastPath
/// \param curPath
///
bool CCompressStrategy::ChangeTagFilePath(const std::string& lastPath, const std::string& curPath)
{
    if(lastPath.empty())
    {
        return false;
    }
    if(curPath.empty())
    {
        return false;
    }

    std::string tagFile = DEBUGTOOL::TEST_TAG_FILE_NAME;

    std::string last_file_path = lastPath + "/" + tagFile;

    if(!check_file_exists(last_file_path))
    {
        return false;
    }
    std::string cur_file_path = curPath + "/" + tagFile;
    // copy file
    std::string cmd = "cp " + last_file_path + " " + cur_file_path;
    int nRes = system(cmd.c_str());
    if(0 != nRes)
    {
        return false;
    }
    return true;
}

///
/// \brief CCompressStrategy::CopyTestTagFile2IssueDir
/// check test tag size and copy it to issue log-data directory
///
void CCompressStrategy::CopyTestTagFile2IssueDir(const std::string& path)
{
    if(path.empty())
    {
        return;
    }
    std::string tagFile = DEBUGTOOL::TEST_TAG_FILE_NAME;

    std::string tagPath = path + "/" + tagFile;

    int nSize = FileSize(tagPath.c_str());
    if( 0 >= nSize)
    {
        return;
    }
    static int nOldSize = 0;

    if(nOldSize == nSize)
    {
        return;
    }

    std::string issDir = GetCurrentIssueLogDir();
    std::string issPath =  path + "/" + issDir;

    //check dir
//    std::string cmd = "if [ -d " + path + " ];then exit 0; else exit 1; fi";
//    int nRes = system(cmd.c_str());
//    if(0 != nRes)
//    {
//        return;
//    }
    if(!check_file_exists(path))
    {
        if(!CreateDirectory(path))
        {
           return;
        }

    }
    if(!check_file_exists(issPath))
    {
        if(!CreateDirectory(issPath))
        {
           return;
        }
    }

    if(copy_file(tagPath, issPath + "/" + tagFile ))
    {
        nOldSize = nSize; ;
    }

//    cmd = "if [  ! -d  " + issPath + "  ];then mkdir -p " + issPath + " ;fi ;";

//    std::string cmd  = "cd \"" + path + "\";cp " + tagFile + " ./" + issDir;

//    int nRes = system(cmd.c_str());

//    if(0 == nRes)
//    {
//        nOldSize = nSize;
//    }

    return;
}

///
/// \brief CCompressStrategy::copy_file
/// \param src_path
/// \param des_path
/// \return
/// copy file interface
///
bool CCompressStrategy::copy_file(const std::string& src_path, const std::string& des_path )
{
    if(src_path.empty())
    {
        return false;
    }
    if(des_path.empty())
    {
        return false;
    }
    if(src_path == des_path)
    {
        return false;
    }
    // read
    std::ifstream file_in;
    file_in.open(src_path.c_str(),  std::ios::in);
    if(!file_in)
    {
        return false;
    }

    // write
    std::ofstream file_out;
    file_out.open(des_path.c_str(),  std::ios::out);
    if(!file_out)
    {
        return false;
    }

    file_out << file_in.rdbuf();


//    std::string temp, sNext="";
//    int nLineCnt = 0;
//    while(std::getline(file_in,temp))
//    {
//        nLineCnt ++;
//        if(nLineCnt > nRemovedCnt)
//        {
//            if(!sNext.empty())
//            {
//                sNext += "\n";
//            }
//            sNext += temp ;
//        }
//    }
    file_in.close();


//    file_out.flush();
//    file_out << sNext << std::endl;
    file_out.close();

    return true;
}
