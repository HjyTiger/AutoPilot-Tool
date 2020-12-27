#ifndef CONFIG_H
#define CONFIG_H
#include "cf_interface.h"
#include <string>
#include <vector>


class TiXmlElement;


namespace DEBUGTOOL
{

using namespace std;

class CAbstractItemInfo
{
    int             m_nItemID;
    std::string     m_sItemName;
    bool            m_bShow;

public:
    explicit CAbstractItemInfo();
    CAbstractItemInfo(const int & id,const std::string & name);
    ~CAbstractItemInfo();

    void SetItemID(const int & id);
    void SetItemName(const string & name);


    const int & GetItemID() const;
    const std::string & GetItemName() const;

    void SetItemShowFlag(const bool& b) ;
    const bool & GetItemShowFlag() const;
};



///
/// \brief The CItemInfo class
/// a single item definition
///
///

class CItemInfo : public CAbstractItemInfo
{
//    int             m_nItemID;
//    std::string     m_sItemName;

//    std::string m_sItemNote;
//    bool            m_bShow;

    bool            m_bNew;

    EIssueEditType  m_eEditType;
    EBaseInfoType   m_eBaseInfoType;

    // related info
    // only for tag-tree 2 tag-table-edit relationship
    void *          m_pRelatedEditTableItem;
    int             m_nRelatedditTableItemRow;

public:
    explicit CItemInfo(void);
    CItemInfo(const int & id,const std::string & name);

    ~CItemInfo(void);

    void SetItemNewFlag(const bool& b);
    const bool & GetItemNewFlag() const;

    void SetItemEditFlag(const EIssueEditType& e);
    const EIssueEditType & GetItemEditFlag() const;

    void SetItemBaseInfoFlag(const EBaseInfoType& e);
    const EBaseInfoType & GetItemBaseInfoFlag() const;

    void SetRelatedEditTableItem( void * p, const int& nline);
    void * GetRelatedEditTableItem(int& nline) const;
};


///
/// \brief The CManageItemStrategy class
///
class CManageItemStrategy
{


    explicit CManageItemStrategy(void) ;
public:
    ~CManageItemStrategy(void);

    CManageItemStrategy(const CManageItemStrategy&) = delete;
    CManageItemStrategy& operator = (const CManageItemStrategy&) = delete;

    static CManageItemStrategy* GetInstance()
    {
        static CManageItemStrategy _instance;
        return &_instance;
    }


    template<typename TItem, typename TIterator>
    TItem * IsHaveItem(const std::string & val, std::vector<TItem*> * pVItems);

    template<typename TItem, typename TIterator>
    TItem * IsHaveItemID(const int & id, std::vector<TItem*> * pVItems);

    template<typename TItem, typename TIterator>
    TItem * AddItem(const std::string & val, std::vector<TItem*> * pVItems, const int& nIndex=-1);

};


///
/// \brief The CItemsGroup class
/// basic items to form a group
///
class CItemsGroup :  public CAbstractItemInfo
{

    vector<CItemInfo*>  m_vItems;

public:
    explicit CItemsGroup();
    CItemsGroup(const int & id,const std::string & name);
    ~CItemsGroup();


    void AddGroupItem( CItemInfo * const p);
    virtual CItemInfo * AddGroupItem(const std::string & val, const int& nIndex=-1);

    CItemInfo * IsHaveItem(const std::string & val);

    const int GetGroupItemsCount() const;
    std::vector<CItemInfo*> * GetGroupItems( );

};

///
/// \brief The CTestTagGroup class
/// specially, only for test tags
///

class CTestTagGroup : public CItemsGroup
{
    EIssueEditType      m_eEditType;

public:
    explicit CTestTagGroup(void);    
    CTestTagGroup(const int & id,const std::string & name);
    ~CTestTagGroup(void);

    CItemInfo * AddGroupItem(const std::string & val, const int& nIndex=-1) override;

    bool SetTestTagGroupEditType(const int & id);
    const EIssueEditType & GetTestTagGroupEditType() const ;
};


//template<typename T>
//class Singleton
//{
//protected:
//    Singleton()
//    {    }

//public:
//    virtual ~Singleton()
//    {    }
//    Singleton(const Singleton&) = delete;
//    Singleton& operator =(const Singleton&) = delete;

//    static T& GetInstance()
//    {
//        static T _instance;
//        return _instance;
//    }
//};

class CConfigFile : public CAbstractItemInfo
{
//    int             m_nID;
//    std::string     m_sProject;
    std::string     m_sFileName;

public:
    explicit CConfigFile();
    CConfigFile(const int & i,const string& name);
    ~CConfigFile();

//    const int GetID() const;
//    const std::string GetProjectName() const;

    const std::string GetConfigFileName() const;
    void SetConfigFileName(const std::string & file);

};

//class CLogConfig : public Singleton<CLogConfig>
class CLogConfig
{
//    friend class Singleton<CLogConfig>;

//    std::string                 m_sFileName;
    std::vector<CTestTagGroup*> m_vTestTagGroups;

//    vector<CItemInfo*>    m_vAddressGroup;
//    vector<CItemInfo*>  m_vDirectionGroup;
//    vector<CItemInfo*>  m_vTakeoverGroup;
//    vector<CItemInfo*>  m_vSwcGroup;
//    vector<CItemInfo*>  m_vCustomGroup;

    vector<CItemsGroup*>        m_vCustomGroups;

    // for multi-project-config
    vector<CConfigFile*>        m_vConfigFiles;
    short                       m_nTreeColumnCount;
    short                       m_nProjectIndex;
    std::string                 m_sConfigFilePath;


    /////------some inital setting
    // delay time set, seconds
    int m_nIssueCheckMaxTimeDelay;

    // enable click tag label
    bool m_bClickTagLableEnabled;

    // enable create new project
    bool m_bCreateNewProjectEnabled;

    // enable create/add new test tag
    bool m_bCreateNewTestTagEnabled;

    // set tree style
    int m_nTestTagTreeColumnStype;

    int m_nMinDiskWarningSize;
    /////-------------------------




    //m_nTextType : 1 from "name" section, default, 2 from "note" section.
    short           m_nTextType;

    std::string     m_sTestTagOperateFileName;

    bool            m_bIsUpdate;
    long long       m_nAllTestTagItemCount;

    // test base infos, 2.0.3 added
//    std::string m_sProject;
    std::string     m_sSWVersionNo;
    std::string     m_sVehichleNo;
    std::string     m_sTester;
    std::string     m_sDiskNo;

    std::string     m_sBaseInfo;
//    std::string     m_sLcmLogFileName;
    std::string     m_sTestTagFileParentPath;

    bool            m_bBaseInfoUpdated;
//    bool            m_bLcmLogFileUpdated;
    bool            m_bTestTagFilePathUpdated;

    int             m_nDefaultFileSize;

    int             m_nOldFileLineCount;


    // compress file setting
    bool            m_bIssueLogCompressed;
    bool            m_bNoIssueLogCompressed;
    short           m_nCompressionCPUCount;


    // private interface
    void InitConfigFiles();
    void InitConfigFileFromDir();
    bool InitConfigFileFromBaseConfig();


    bool DeleteAllChilds(TiXmlElement * const pnt);
    bool SaveItemInfoNodes( TiXmlElement * const pnt,vector<CItemInfo*> & vData , const std::string & itemName);
    bool SaveBaseConfigNode(TiXmlElement * const base, vector<CItemInfo*> & vData, const std::string & nodeName);

    bool LoadItemInfoNodes( TiXmlElement * const pnt, vector<CItemInfo*> & vData , const std::string & itemName);
    bool LoadBaseConfigNode(TiXmlElement * const base, vector<CItemInfo*> & vData, const std::string & nodeName );

    bool GetTagOperateFileName(  std::string & filePath);

    void DeleteGroup(vector<CItemInfo*> & grp);

    void TestTagFile_RemoveOld(const std::string& filePath, int& nRemovedCnt);

    bool GetUpdatedString(std::string &sCurLine, const std::string & text, const int& nType);

    template<typename TGroup, typename TIterator>
    bool SaveItemsGroup(TiXmlElement*& ttags, vector<TGroup*> &  vGroup);

    void ClearConfigInfos();
    bool SaveConfigFile_SingleInfo();

    bool CopyFile(const std::string& desFile, const std::string& orgFile );

//public:
    CLogConfig(void) ;
public:
    ~CLogConfig(void);

    CLogConfig(const CLogConfig&) = delete;
    CLogConfig& operator = (const CLogConfig&) = delete;

    static CLogConfig* GetInstance()
    {
        static CLogConfig _instance;
        return &_instance;
    }

    bool LoadConfigFile(const std::string &sParentPath);
    bool SaveConfigFile(const EConfigFileNodeType& eType = CNT_DEFAULT);

    std::vector<CTestTagGroup*> *  GetTestTagGroups(  ) ;
    CTestTagGroup* AddTestTagGroup(const std::string& name);
    CTestTagGroup*  GetTestTagGroupItems( const int& nID );

    CItemsGroup*   GetBaseGroupItems( const int& nID );

    bool SetTestTreeColumnCount(const int& cnt, const bool & bFileUpdated);
    const int  GetTestTreeColumnCount() const;

    bool SaveBaseInfoText();
    bool SaveTestTagOperateFile(const std::string & time, const int& parentID, CItemInfo * const pinfo, const bool & checked, const std::string & sTag, int & nLineIndex, const std::string& sFile, int & nRemoved);
    bool UpdateTestTagText(const int & nLineIndex,const std::string & text,  const int& nType,const std::string & text_2="", const int& nType_2=-1);
    bool GetTestTagFileCount(const std::string& filePath);

    void UpdateLogConfig(const bool& b,  const EConfigFileNodeType& eType = CNT_DEFAULT);

    const int GetCurrentTestTagItemCount() const;
    void UpdateCurrentTestTagItemCount() ;

//    void SetLcmLogFileName(const std::string & fileName);
    void SetTestBaseInfo(const std::string & info);
    void SetTestTagFileParentPath(const std::string & path);

    CItemInfo * AddBaseInfoItem(const EBaseInfoType& eType, const std::string& val);

    // test base infos, for saved in config file, 2.0.3 added
    bool SetTestBaseRecordInfos(const ETestBaseInfos& eType, const std::string& val);
    std::string GetTestBaseRecordInfos(const ETestBaseInfos& eType);

    // follow: for project-xml config setting
    const std::vector<CConfigFile*> *  GetConfigFilesGroup(  ) const;

    const std::string GetConfigFile(void ) const ;
    const std::string GetProjectName(void ) const;
    const int GetProjectID() const;
    const int GetProjectIndex() const;
    bool SetProjectIndex(const int  & n);

    std::string GetEditTypeName(const EIssueEditType & editType);

    bool CheckProjectExist(const std::string& project);
    void AddConfigFile(const std::string& prjName, const std::string& template_name);

    bool check_file_exists (const std::string& name);


    bool SaveProjectFile( const EProjectFileNodeType& eType);

    // some initial setting

    inline int GetIssueCheckMaxTimeDelay()  const   {  return  m_nIssueCheckMaxTimeDelay;  }

    // enable click tag label
    inline bool GetClickTagLableEnabled()   const   {   return  m_bClickTagLableEnabled;    }

    // enable create new project
    inline bool GetCreateNewProjectEnabled()    const   {   return  m_bCreateNewProjectEnabled; }

    // enable create/add new test tag
    inline bool GetCreateNewTestTagEnabled()    const   {   return  m_bCreateNewTestTagEnabled; }

    // set tree style
    inline int GetTestTagTreeColumnStype() const   {   return  m_nTestTagTreeColumnStype;  }

    inline int GetMinDiskWarningSize() const   {   return  m_nMinDiskWarningSize;  }


    // compress log setting
    bool SetIssueLogCompressed(const bool& bCompressed);
    bool SetNoIssueLogCompressed(const bool& bCompressed);
    bool SetCompressedCPUCount(const short& cnt);
    inline bool GetIssueLogCompressed() const                        {        return m_bIssueLogCompressed;            }
    inline bool GetNoIssueLogCompressed() const                      {        return m_bNoIssueLogCompressed;          }
    inline short GetCompressedCPUCount() const                       {        return m_nCompressionCPUCount;           }
};


///
///  player needed
///
///
/// \brief The CTagInfo class
///

class CTagInfo: public CAbstractItemInfo
{
    std::string     m_sStartTime;
    std::string     m_sEndTime;

    int             m_nProjectID;
    int             m_nIssueTypeID;

    std::string     m_sLogNames;

public:
    explicit CTagInfo();
    ~CTagInfo();

    void SetStartTime(const std::string& tm) ;
    const std::string GetStartTime()const;

    void SetEndTime(const std::string& tm) ;
    const std::string GetEndTime()const;

    void SetLogNames(const std::string& s) ;
    const std::string GetLogNames()const;

    void SetProjectID(const int& );
    const int GetProjectID()const;

    void SetIssueTypeID(const int& );
    const int GetIssueTypeID()const;

    std::string GetTagInfo(const int & type) const;
};



///
/// \brief The CPlayConfig class
///
class CPlayConfig
{
    std::string             m_sTestTagFile;
//    std::string m_sLogPath
    vector<CTagInfo* >      m_vTagInfos;
    std::string             m_sBaseInfo;

    vector<CTagInfo* >      m_vCurrentLogTagInfos;



    bool OpenTestTagsFile();
    void StringTrim(  std::string& str);

    bool CheckLogFileFormat(const std::string fileName, const short& type);
    bool CheckLogExist(const std::string& fileName, vector<std::string>& vFiles);

    void ClearTagGroup();

    //public:
    explicit CPlayConfig(void) ;
public:
    ~CPlayConfig(void);
    CPlayConfig(const CPlayConfig&) = delete;
    CPlayConfig& operator = (const CPlayConfig&) = delete;

    static CPlayConfig* GetInstance()
    {
        static CPlayConfig _instance;
        return &_instance;
    }


    bool GetLogFileNames(const std::string& parentPath, vector<std::string>& vFiles);
    bool SetTestTagFileName(const std::string& file);
    vector<CTagInfo* >* GetLogTestTags(const long int & nStartTime, const long int & nStopTime);
    std::string GetBaseInfo() const;
    std::string GetTestTagFileName() const;

};


}

#endif
