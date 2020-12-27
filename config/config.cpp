#include "config.h"

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#include "tinyxml.h"
#endif

#include <iostream>
#include <fstream>
#include <algorithm>

#include <dirent.h>

#include <iomanip>

#include <stdlib.h>

using namespace DEBUGTOOL;

//------------------
//
//--CTestTagGroup
//
//------------------


CAbstractItemInfo::CAbstractItemInfo()
    : m_bShow(true)
    , m_nItemID(-1)
    , m_sItemName("")
{

}
CAbstractItemInfo::CAbstractItemInfo(const int & id,const std::string & name)
    : m_bShow(true)
    , m_nItemID(id)
    , m_sItemName(name)
{

}
CAbstractItemInfo::~CAbstractItemInfo()
{

}

void CAbstractItemInfo::SetItemID(const int & id)
{
    m_nItemID = id;
}

void CAbstractItemInfo::SetItemName(const string & name)
{
    m_sItemName = name;
}

const int & CAbstractItemInfo::GetItemID() const
{
    return m_nItemID;
}

const std::string & CAbstractItemInfo::GetItemName() const
{
    return m_sItemName;
}

void CAbstractItemInfo::SetItemShowFlag(const bool& b)
{
    m_bShow = b;
}

const bool & CAbstractItemInfo::GetItemShowFlag() const
{
    return m_bShow;
}



///
/// \brief CItemInfo::CItemInfo
///
CItemInfo::CItemInfo(void)
    : CAbstractItemInfo()
    , m_bNew(false)
    , m_eEditType(EIT_DEFAULT_NO)
    , m_eBaseInfoType(EBT_DEFAULT_NO)
    , m_pRelatedEditTableItem(NULL)
    , m_nRelatedditTableItemRow(-1)
//    , m_nItemID(-1)
{

}

CItemInfo::CItemInfo(const int & id,const std::string & name)
    : CAbstractItemInfo(id,name)
    , m_bNew(false)
    , m_eEditType(EIT_DEFAULT_NO)
    , m_eBaseInfoType(EBT_DEFAULT_NO)
    , m_pRelatedEditTableItem(NULL)
    , m_nRelatedditTableItemRow(-1)
{

}

CItemInfo::~CItemInfo(void)
{

}



void CItemInfo::SetItemNewFlag(const bool& b)
{
    m_bNew = b;
}

const bool & CItemInfo::GetItemNewFlag() const
{
    return m_bNew;
}

void CItemInfo::SetItemEditFlag(const EIssueEditType& b)
{
    m_eEditType = b;
}

const EIssueEditType & CItemInfo::GetItemEditFlag() const
{
    return m_eEditType;
}

void CItemInfo::SetItemBaseInfoFlag(const EBaseInfoType& e)
{
    m_eBaseInfoType = e;
}

const EBaseInfoType & CItemInfo::GetItemBaseInfoFlag() const
{
    return m_eBaseInfoType;
}

void CItemInfo::SetRelatedEditTableItem( void * p, const int & nline)
{
    m_pRelatedEditTableItem = p;
    m_nRelatedditTableItemRow = nline;
}

void * CItemInfo::GetRelatedEditTableItem(int & nline) const
{
    nline = m_nRelatedditTableItemRow;
    return m_pRelatedEditTableItem;
}

///
/// \brief CManageItemStrategy::CManageItemStrategy
///
CManageItemStrategy::CManageItemStrategy()
{

}

CManageItemStrategy::~CManageItemStrategy()
{

}

template<typename TItem, typename TIterator>
TItem * CManageItemStrategy::IsHaveItem(const std::string & val, std::vector<TItem*> * pVItems)
{
    TItem * pItem = NULL;
    for(TIterator it = pVItems->begin(); it != pVItems->end(); ++it)
    {
        if(*it)
        {
            std::string sName = (*it)->GetItemName();
            if(sName == val)
            {
                pItem = (*it);
                break;
            }
        }
    }

    return pItem;
}


template<typename TItem, typename TIterator>
TItem * CManageItemStrategy::IsHaveItemID(const int & id, std::vector<TItem*> * pVItems)
{
    TItem * pItem = NULL;
    for(TIterator it = pVItems->begin(); it != pVItems->end(); ++it)
    {
        if(*it)
        {
            int nID = (*it)->GetItemID();
            if(nID == id)
            {
                pItem = (*it);
                break;
            }
        }
    }

    return pItem;
}


template<typename TItem, typename TIterator>
TItem * CManageItemStrategy::AddItem(const std::string & val, std::vector<TItem*> * pVItems, const int& nIndex)
{
    TItem * pItem = IsHaveItem<TItem, TIterator>(val,pVItems );
    if(NULL != pItem)
    {
        if(pItem->GetItemShowFlag())
        {
            return NULL;
        }
        else
        {
            pItem->SetItemShowFlag(true);
            return pItem;
        }
    }

    int curItemIndex = 0;
    if(-1 == nIndex)
    {
        curItemIndex =  pVItems->size()+1;
    }
    else
    {
        curItemIndex = nIndex;
    }
    TItem * item = new TItem(curItemIndex, val);

    pVItems->push_back(item);

    return item;
}


///
/// \brief CBaseInfoGroup::CBaseInfoGroup
///

CItemsGroup::CItemsGroup()
    : CAbstractItemInfo()
{

}

CItemsGroup::CItemsGroup(const int & id,const std::string & name)
    : CAbstractItemInfo(id,name)
{

}

CItemsGroup::~CItemsGroup()
{
    for(std::vector<CItemInfo*>::iterator it = m_vItems.begin(); it != m_vItems.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
        }
    }
    m_vItems.clear();
    m_vItems.shrink_to_fit();
}


CItemInfo * CItemsGroup::IsHaveItem(const std::string & val)
{
    CItemInfo * pItem = NULL;
    for(std::vector<CItemInfo*>::iterator it = m_vItems.begin(); it != m_vItems.end(); ++it)
    {
        if(*it)
        {
            std::string sName = (*it)->GetItemName();
            if(sName == val)
            {
                pItem = (*it);
                break;
            }
        }
    }

    return pItem;
}

void CItemsGroup::AddGroupItem( CItemInfo * const item)
{
    m_vItems.push_back(item);
}

CItemInfo * CItemsGroup::AddGroupItem(const std::string & val, const int& nIndex)
{
    CItemInfo * item = NULL;
    CManageItemStrategy * pStrategy = CManageItemStrategy::GetInstance();
    if(NULL != pStrategy)
    {
        item = pStrategy->AddItem<CItemInfo , std::vector<CItemInfo*>::iterator>(val,&m_vItems,nIndex);
    }





//   SaveItemsGroup<CTestTagGroup,vector<CTestTagGroup*>::iterator>(ttags, m_vTestTagGroups);

//    CItemInfo * pItem = IsHaveItem(val);
//    if(NULL != pItem)
//    {
//        if(pItem->GetItemShowFlag())
//        {
//            return NULL;
//        }
//        else
//        {
//            pItem->SetItemShowFlag(true);
//            return pItem;
//        }
//    }

//    int curItemIndex = 0;
//    if(-1 == nIndex)
//    {
//        curItemIndex =  GetGroupItemsCount()+1;
//    }
//    else
//    {
//        curItemIndex = nIndex;
//    }
//    CItemInfo * item = new CItemInfo(curItemIndex, val);

//    AddGroupItem(item);

    return item;
}

vector<CItemInfo*> * CItemsGroup::GetGroupItems()
{
    return &m_vItems;
}

const int CItemsGroup::GetGroupItemsCount() const
{
    return m_vItems.size();
}



//------------------
//
//--CTestTagGroup
//
//------------------

CTestTagGroup::CTestTagGroup(void)
    : CItemsGroup()
    , m_eEditType(EIT_DEFAULT_NO)
{

}

CTestTagGroup::CTestTagGroup(const int & id,const std::string & name)
    : CItemsGroup(id,name)
    , m_eEditType(EIT_DEFAULT_NO)
{

}

CTestTagGroup::~CTestTagGroup(void)
{

}

CItemInfo * CTestTagGroup::AddGroupItem(const std::string & val, const int& nIndex)
{
    int curItemIndex = GetItemID()*100 + CItemsGroup::GetGroupItemsCount()+1;

    return CItemsGroup::AddGroupItem(val,curItemIndex);
}

bool CTestTagGroup::SetTestTagGroupEditType(const int & id)
{
    bool bUpdated = false;
    if( id != m_eEditType)
    {
        m_eEditType = (EIssueEditType)id;
        bUpdated = true;
    }
    return bUpdated;
}

const EIssueEditType & CTestTagGroup::GetTestTagGroupEditType() const
{
    return m_eEditType;
}


///
/// \brief CConfigFile::CConfigFile
///
///  new group
///

CConfigFile::CConfigFile()
    : CAbstractItemInfo()
    , m_sFileName()
{

}

CConfigFile::CConfigFile(const int& id, const std::string& name)
    : CAbstractItemInfo(id,name)
{

//    fileName = name;

//    std::string path = "./config";
//    std::transform(fileName.begin(),fileName.end(), fileName.begin(), ::tolower);

//    fileName = path + "/config-" + fileName + ".xml";
}

CConfigFile::~CConfigFile()
{

}

//const int CConfigFile::GetID() const
//{

//}
//const std::string CConfigFile::GetProjectName() const
//{
//    return m_sProject;
//}

const std::string CConfigFile::GetConfigFileName() const
{
    return m_sFileName;
}

void CConfigFile::SetConfigFileName(const std::string & file)
{
    m_sFileName = file;
}

//------------------
//
//--CLogConfig
//  singeton config class for logger
//
//------------------

CLogConfig::CLogConfig(void)
    : m_nTreeColumnCount(6)
    , m_sTestTagOperateFileName(DEBUGTOOL::TEST_TAG_FILE_NAME)
    , m_nTextType(1)
    , m_bIsUpdate(false)
    , m_nAllTestTagItemCount(0)
    //  , m_sProject()
    , m_sSWVersionNo("V0.0.0")
    , m_sVehichleNo("00")
    , m_sTester("00")
    , m_sDiskNo("00")
    , m_sBaseInfo("")
//    , m_sLcmLogFileName()
    , m_sTestTagFileParentPath()
    , m_bBaseInfoUpdated(false)
//    , m_bLcmLogFileUpdated(false)
    , m_bTestTagFilePathUpdated(false)
    , m_nDefaultFileSize(500)
    , m_nOldFileLineCount(-1)
    , m_nProjectIndex(-1)
    , m_sConfigFilePath("")    
    , m_nIssueCheckMaxTimeDelay(5)
    , m_bClickTagLableEnabled(false)
    , m_bCreateNewProjectEnabled(false)
    , m_bCreateNewTestTagEnabled(false)
    , m_nTestTagTreeColumnStype(0)
    , m_nMinDiskWarningSize(3)
    , m_bIssueLogCompressed(true)
    , m_bNoIssueLogCompressed(false)
    , m_nCompressionCPUCount(1)
{
    InitConfigFiles();
}

CLogConfig::~CLogConfig(void)
{
    for(std::vector<CConfigFile*>::iterator it = m_vConfigFiles.begin(); it != m_vConfigFiles.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
        }
    }
    m_vConfigFiles.clear();
    m_vConfigFiles.shrink_to_fit();

    ClearConfigInfos();
}

void CLogConfig::ClearConfigInfos()
{
    for(std::vector<CTestTagGroup*>::iterator it = m_vTestTagGroups.begin(); it != m_vTestTagGroups.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
        }
    }
    m_vTestTagGroups.clear();
    m_vTestTagGroups.shrink_to_fit();

    for(std::vector<CItemsGroup*>::iterator it = m_vCustomGroups.begin(); it != m_vCustomGroups.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
        }
    }
    m_vCustomGroups.clear();
    m_vCustomGroups.shrink_to_fit();
}

void CLogConfig::DeleteGroup(vector<CItemInfo*> & grp)
{
    for(std::vector<CItemInfo*>::iterator it = grp.begin(); it != grp.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
        }
    }
}

///
/// \brief CLogConfig::InitConfigFiles
/// init config-file group,
///
void CLogConfig::InitConfigFiles()
{
    if(!InitConfigFileFromBaseConfig())
    {
        InitConfigFileFromDir();
    }

}

///
/// \brief CLogConfig::InitConfigFileFromBaseConfig
/// init config-file group, from base-config file
/// read base-config file and check dir file together
///
bool CLogConfig::InitConfigFileFromBaseConfig()
{
    std::string path = "./config";
    std::string filePath = path + "/project-base.xml";
    if (filePath.empty())
    {
        return false;
    }

    TiXmlDocument doc;
    if(!doc.LoadFile(filePath.c_str()))
    {
        return false;
    }

    // top level node

    TiXmlElement* root = doc.FirstChildElement("Config");
    if (0 == root)
    {
        return false;
    }


    // new node
    TiXmlElement* pj = root->FirstChildElement("Project");
    if (0 != pj)
    {
        int did = -1;
        pj->QueryIntAttribute("did",&did);

        for (TiXmlElement* subPrj = pj->FirstChildElement("SubProject"); subPrj; subPrj = subPrj->NextSiblingElement())
        {
            if(0 == subPrj)
            {
                continue;
            }

            for (TiXmlElement* itm = subPrj->FirstChildElement("Item"); itm; itm = itm->NextSiblingElement())
            {

                int id = -1;
                itm->QueryIntAttribute("id",&id);
                std::string name = itm->Attribute("name");

                CConfigFile* citem = new CConfigFile(id, name);
                m_vConfigFiles.push_back(citem);

                std::string file = itm->Attribute("file");
                citem->SetConfigFileName(file);

                if(id == did)
                {
                    m_nProjectIndex = m_vConfigFiles.size()-1;
                }
            }
        }
    }

    TiXmlElement* log = root->FirstChildElement("Logger");
    if(0 != log)
    {
        int id = -1;
        log->QueryIntAttribute("delayTime",&id);
//        DEBUGTOOL::ISSUE_CHECK_MAX_TIME_DELAY = id;
        m_nIssueCheckMaxTimeDelay = id;

        id = -1;
        log->QueryIntAttribute("enableClickTagLabel",&id);
//        DEBUGTOOL::CLICK_TAG_LABLE_ENABLED = (1==id);
        m_bClickTagLableEnabled = ( 1 == id );

        id = -1;
        log->QueryIntAttribute("enableCreateNewProject",&id);
//        DEBUGTOOL::CREATE_NEW_PROJECT_ENABLED = (1==id);
        m_bCreateNewProjectEnabled = ( 1 == id );


        id = -1;
        log->QueryIntAttribute("enableAddNewTag",&id);
//        DEBUGTOOL::CREATE_NEW_TESTTAG_ENABLED = (1==id);
        m_bCreateNewTestTagEnabled = ( 1 == id );


        id = -1;
        log->QueryIntAttribute("tagTreeUIStype",&id);
//        DEBUGTOOL::TEST_TAG_TREE_COLUMN_STYLE = (1==id);
        m_nTestTagTreeColumnStype = id ;

        id = -1;
        log->QueryIntAttribute("minDiskWarningSize",&id);
//        DEBUGTOOL::TEST_TAG_TREE_COLUMN_STYLE = (1==id);
        m_nMinDiskWarningSize = id ;
     }

    if(m_vConfigFiles.size()>0)
    {
        if(0 > m_nProjectIndex)
        {
            m_nProjectIndex = 0;
        }
        m_sConfigFilePath = path;
    }

    // load test base infos record
    TiXmlElement* tbase = root->FirstChildElement("TestBaseInfos");
    if(0 != tbase)
    {
        SetTestBaseRecordInfos(TBI_SW_Version_No,tbase->Attribute("SWVersionNo"));
        SetTestBaseRecordInfos(TBI_Vehicle_No,tbase->Attribute("vehicleNo"));
        SetTestBaseRecordInfos(TBI_Tester,tbase->Attribute("tester"));
        SetTestBaseRecordInfos(TBI_Disk_No,tbase->Attribute("diskNo"));
    }

    //compress setting
    TiXmlElement* comp = root->FirstChildElement("CompressSetting");
    if(0 != comp)
    {
        int id = -1;
        comp->QueryIntAttribute("issueLogCompressed",&id);
        m_bIssueLogCompressed = (1==id);

        id = -1;
        comp->QueryIntAttribute("noIssueLogCompressed",&id);
        m_bNoIssueLogCompressed = (1==id);

        id = -1;
        comp->QueryIntAttribute("cpuCount",&id);
        m_nCompressionCPUCount = id;
    }

    return true;
}

///
/// \brief CLogConfig::SaveProjectFile
/// \param eType
/// \return
///  according type,save project default values
///
bool CLogConfig::SaveProjectFile( const EProjectFileNodeType& eType)
{
    std::string path = "./config";
    std::string filePath = path + "/project-base.xml";
    if (filePath.empty())
    {
        return false;
    }

    TiXmlDocument doc;
    if(!doc.LoadFile(filePath.c_str()))
    {
        return false;
    }

    // top level node

    TiXmlElement* root = doc.FirstChildElement("Config");
    if (0 == root)
    {
        return false;
    }

    if( PNT_Project_Default == eType)
    {
        // new node
        TiXmlElement* pj = root->FirstChildElement("Project");
        if (0 == pj )
        {
            pj = new TiXmlElement("Project");
            root->LinkEndChild( pj );
        }

        if(0 <= m_nProjectIndex && m_nProjectIndex < m_vConfigFiles.size())
        {
            int did = -1;
            did = m_vConfigFiles[m_nProjectIndex]->GetItemID();
            pj->SetAttribute("did",did);
        }

    }
    else if(PNT_TestBaseInfo_Default == eType)
    {
        TiXmlElement* el = root->FirstChildElement("TestBaseInfos");

        if(0 == el)
        {
            el = new TiXmlElement("TestBaseInfos");
            root->LinkEndChild( el );
        }

        el->SetAttribute("SWVersionNo",GetTestBaseRecordInfos(TBI_SW_Version_No));
        el->SetAttribute("vehicleNo",GetTestBaseRecordInfos(TBI_Vehicle_No));
        el->SetAttribute("tester",GetTestBaseRecordInfos(TBI_Tester));
        el->SetAttribute("diskNo",GetTestBaseRecordInfos(TBI_Disk_No));
    }
    else if(PNT_CompressSetting_Default == eType)
    {
        TiXmlElement* el = root->FirstChildElement("CompressSetting");

        if(0 == el)
        {
            el = new TiXmlElement("CompressSetting");
            root->LinkEndChild( el );
        }

        el->SetAttribute("issueLogCompressed",m_bIssueLogCompressed ? 1 : 0);
        el->SetAttribute("noIssueLogCompressed",m_bNoIssueLogCompressed ? 1 : 0);
        el->SetAttribute("cpuCount",m_nCompressionCPUCount);

    }

    doc.SaveFile();
    return true;
}

//
/// \brief CLogConfig::InitConfigFileFromDir
/// get all config-file, from dir reading
///
void CLogConfig::InitConfigFileFromDir()
{
    std::string parentPath = "./config";

    int nType = 0;

    DIR *dir;
    struct dirent *ptr;

    dir = opendir(parentPath.c_str());
    if(NULL == dir)
    {
        return;
    }
    int idx = 0;
    while(NULL != (ptr = readdir(dir)))
    {
        if(ptr->d_name[0] == '.')
        {
            continue;
        }
        if(ptr->d_type != 8) //file
        {
            return;
        }

        std::string fileName = ptr->d_name;

        int nS = fileName.find_first_of("-");
        int nE = fileName.find_first_of(".xml");
        if(nE < 0)
        {
            continue;
        }
        std::string suf = fileName.substr(nE,4);
        if(suf != ".xml")
        {
            continue;
        }
        int nSuf = fileName.length()-4;
        if(nE != nSuf)
        {
            continue;
        }

        std::string prjName = fileName.substr(nS+1,nE-nS-1);
        if(!prjName.empty() && nS > 0 && nE > 0)
        {
            std::transform(prjName.begin(),prjName.end(), prjName.begin(), ::toupper);
        }
        else
        {
            prjName="default";
            nType = idx;
        }

        if(prjName == "YSG")
        {
            nType = idx;
        }

        CConfigFile* pfile = new CConfigFile(idx,prjName);
        pfile->SetConfigFileName(parentPath+"/"+fileName);
        m_vConfigFiles.push_back(pfile);

        idx++;
    }
    closedir(dir);


//    std::string cur_file="";
//    if(2 == nType)
//    {
//        cur_file = parentPath+"/config-ysg.xml";
//    }
//    else if(1 == nType)
//    {
//        cur_file = parentPath+"/config.xml";
//    }
//    else
//    {
        if(m_vConfigFiles.size()>0)
        {
            m_nProjectIndex = nType;
            m_sConfigFilePath = parentPath;
//            m_sFileName = m_vConfigFiles[0]->GetConfigFileName();
        }
//    }
}


///
/// \brief CLogConfig::GetConfigFilesGroup
/// \return
/// return all config group point
///
const std::vector<CConfigFile*> *  CLogConfig::GetConfigFilesGroup() const
{
    return &m_vConfigFiles;
}

///
/// \brief CLogConfig::CheckProjectExist
/// \param project
/// \return
/// check if exist
///
bool CLogConfig::CheckProjectExist(const std::string& project)
{
    bool bfind = false;
    for(std::vector<CConfigFile*>::iterator it = m_vConfigFiles.begin(); it != m_vConfigFiles.end(); ++it)
    {
        if(*it)
        {
            if((*it)->GetItemName() == project)
            {
                bfind = true;
                break;
            }
        }
    }
    return bfind;
}


///
/// \brief CLogConfig::AddConfigFile
/// \param project
/// \param template_file
/// add new config-xxx.xml file
///
void CLogConfig::AddConfigFile(const std::string& project_org, const std::string& template_file)
{
    std::string project = project_org;
    std::transform(project.begin(),project.end(), project.begin(), ::tolower);

    std::string prjName = project;
    std::transform(prjName.begin(),prjName.end(), prjName.begin(), ::toupper);

    if(!CheckProjectExist(prjName))
    {
        // create new file
        std::string parentPath = m_sConfigFilePath; //"./config";

        std::string fileName = "config-"+project + ".xml";

        CopyFile(parentPath + "/" + fileName, parentPath + "/" + template_file);

        int idx = m_vConfigFiles.size();

        CConfigFile* pfile = new CConfigFile(idx,prjName);
        pfile->SetConfigFileName(fileName);
        m_vConfigFiles.push_back(pfile);

        m_nProjectIndex = idx;

    //    m_sFileName = fileName;
    }
}

///
/// \brief CLogConfig::CopyFile
/// \param desFile
/// \param orgFile
/// \return
/// copy file simply
///
bool CLogConfig::CopyFile(const std::string& desFile, const std::string& orgFile )
{
    // read
    std::ifstream file_in;
    file_in.open(orgFile.c_str(),  std::ios::in);

    std::string temp, sNext="";
    while(std::getline(file_in,temp))
    {
        if(!sNext.empty())
        {
            sNext += "\n";
        }
        sNext += temp ;
    }
    file_in.close();

    // write
    std::ofstream file_out;
    file_out.open(desFile.c_str(),  std::ios::out);
    file_out.flush();
    file_out << sNext << std::endl;
    file_out.close();

    return true;
}

///
/// \brief CLogConfig::SetProjectIndex
/// \param idx
/// \return
/// set current project index
///
bool CLogConfig::SetProjectIndex(const int & idx)
{
    bool bUpdated = false;
    if( idx != m_nProjectIndex)
    {
        m_nProjectIndex = idx;
        bUpdated = true;

        SaveProjectFile(PNT_Project_Default);
    }

    m_nProjectIndex = idx;

    return bUpdated;
}

///
/// \brief CLogConfig::GetProjectIndex
/// \return
/// get current project index
///
const int CLogConfig::GetProjectIndex() const
{
    return m_nProjectIndex;
}

///
/// \brief CLogConfig::GetConfigFile
/// \return
/// get current config-xxx.xml file name
///
const std::string CLogConfig::GetConfigFile(void ) const
{
    std::string name = "";
    if(m_nProjectIndex >= 0 && m_nProjectIndex < m_vConfigFiles.size())
    {
        name = m_sConfigFilePath + "/" + m_vConfigFiles[m_nProjectIndex]->GetConfigFileName();
    }

    return name;
}

///
/// \brief CLogConfig::GetProjectName
/// \return
/// get current project name
///
const std::string CLogConfig::GetProjectName(void ) const
{
    std::string name="";
    if(m_nProjectIndex >= 0 && m_nProjectIndex < m_vConfigFiles.size())
    {
        name = m_vConfigFiles[m_nProjectIndex]->GetItemName();
    }

//    for(std::vector<CConfigFile*>::const_iterator it = m_vConfigFiles.begin(); it != m_vConfigFiles.end(); ++it)
//    {
//        if(m_sFileName == (*it)->GetConfigFileName())
//        {
//            name = (*it)->GetItemName();
//        }
//    }
    return name;
}

///
/// \brief CLogConfig::GetProjectID
/// \return
/// get current project id
const int CLogConfig::GetProjectID() const
{
    int id = -1;
    if(m_nProjectIndex >= 0 && m_nProjectIndex < m_vConfigFiles.size())
    {
        id = m_vConfigFiles[m_nProjectIndex]->GetItemID();
    }
    return id;
}

///
/// \brief CLogConfig::UpdateLogConfig
/// \param bUpdate
/// \param eType
/// update current config-xxx.xml, according current updated values to save it
///
void CLogConfig::UpdateLogConfig(const bool& bUpdate, const EConfigFileNodeType& eType)
{
    m_bIsUpdate = bUpdate;

    if(m_bIsUpdate)
    {
        SaveConfigFile(eType);
    }
}

///
/// \brief CLogConfig::GetTestTagGroups
/// \return
/// get current test tag groups
///
std::vector<CTestTagGroup*> *  CLogConfig::GetTestTagGroups(  )
{
    return &m_vTestTagGroups;
}

///
/// \brief CLogConfig::GetTestTagGroupItems
/// \param nID
/// \return
///
CTestTagGroup*  CLogConfig::GetTestTagGroupItems( const int& nID )
{
    CTestTagGroup*  pgrp = NULL;

    for (std::vector<CTestTagGroup*>::iterator it = m_vTestTagGroups.begin(); it != m_vTestTagGroups.end(); ++it)
    {
        int nGrpID = (*it)->GetItemID();
        if( nGrpID == nID)
        {
            pgrp = (*it);
            break;
        }
    }

    return pgrp;
}

///
/// \brief CLogConfig::AddTestTagGroup
/// \param val
/// \return
/// add new test tag group
///
CTestTagGroup* CLogConfig::AddTestTagGroup(const std::string& val)
{
    CTestTagGroup * pItem = NULL;
    CManageItemStrategy * pStrategy = CManageItemStrategy::GetInstance();
    if(NULL != pStrategy )
    {
        pItem = pStrategy->AddItem<CTestTagGroup , std::vector<CTestTagGroup*>::iterator>(val,&m_vTestTagGroups);
    }

//    delete pStrategy;
    return pItem;
}

///
/// \brief CLogConfig::GetBaseGroupItems
/// \param nID
/// \return
/// get base group items in current config-xxx.xml
///
CItemsGroup*  CLogConfig::GetBaseGroupItems( const int& nID )
{
    CItemsGroup*  pgrp = NULL;

    for (std::vector<CItemsGroup*>::iterator it = m_vCustomGroups.begin(); it != m_vCustomGroups.end(); ++it)
    {
        int nGrpID = (*it)->GetItemID();
        if( nGrpID == nID)
        {
            pgrp = (*it);
            break;
        }
    }

    return pgrp;
}

///
/// \brief CLogConfig::AddBaseInfoItem
/// \param eType
/// \param val
/// \return
/// add new base info item
///
CItemInfo * CLogConfig::AddBaseInfoItem(const EBaseInfoType& eType, const std::string& val)
{
    CItemsGroup* pGrp = GetBaseGroupItems( eType );

    if(NULL == pGrp)
    {
        std::string name = std::to_string((int)(eType - EBT_CUSTOM));

        CItemsGroup* pBaseGrp = new CItemsGroup(eType, "custom_"+ name );
        m_vCustomGroups.push_back(pBaseGrp);
        pGrp = pBaseGrp;
    }

    return pGrp->AddGroupItem(val);
}

///
/// \brief CLogConfig::GetEditTypeName
/// \param editType
/// \return
/// according edit-type to get type's name
///
std::string CLogConfig::GetEditTypeName(const EIssueEditType & editType)
{
    std::string name="";
    switch(editType)
    {
    case EIT_DEFAULT_NO:
        name = "无Issue添加编辑选项";
        break;
    case EIT_EDIT:
        name = "添加Issue编辑框";
        break;
    case EIT_COMBO_EDIT:
        name = "YSG定制补充Issue编辑框";
        break;
    case EIT_ISSUEADD_SUPPLY_CUSTOM:
        name = "添加Issue编辑框,问题补充组合框";
        break;
    case EIT_SUPPLY_CUSTOM:
        name = "问题补充组合框";
        break;
    case EIT_EDIT_IN_ISSUE:
        name = "单个Issue问题补充编辑框";
        break;
    case EIT_COMBO_IN_ISSUE:
        name = "单个Issue问题补充组合框";
        break;
    }
    return name;
}

///
/// \brief CLogConfig::SetTestTreeColumnCount
/// \param cnt
/// \param bFileUpdated
/// \return
/// set current test tag show tree column count, this value will be saved in config-xxx.xml file
///
bool CLogConfig::SetTestTreeColumnCount(const int& cnt, const bool & bFileUpdated)
{
    bool bUpdated = false;
    if( m_nTreeColumnCount != cnt)
    {
        m_nTreeColumnCount = cnt;
        bUpdated = true;
        SaveConfigFile(CNT_Project_Base);
    }
    else
    {
        if(bFileUpdated)
        {
            SaveConfigFile(CNT_Project_Base);
        }
    }
    return bUpdated;
}

///
/// \brief CLogConfig::GetTestTreeColumnCount
/// \return
/// get current test tag show tree column count
///
const int  CLogConfig::GetTestTreeColumnCount() const
{
    return m_nTreeColumnCount;
}

///
/// \brief CLogConfig::GetCurrentTestTagItemCount
/// \return
/// del?
const int CLogConfig::GetCurrentTestTagItemCount() const
{
    return m_nAllTestTagItemCount;
}

///
/// \brief CLogConfig::UpdateCurrentTestTagItemCount
/// del?
void CLogConfig::UpdateCurrentTestTagItemCount()
{
    m_nAllTestTagItemCount++;
}

///
/// \brief CLogConfig::LoadConfigFile
/// \param sParentPath
/// \return
/// open and load current config-xxx.xml file
///
bool CLogConfig::LoadConfigFile(const std::string &sParentPath)
{        
    if (!sParentPath.empty())
    {
        m_sConfigFilePath = sParentPath + "/config";
    }

    std::string fileName =  GetConfigFile();

    if (fileName.empty() || 0 == fileName.size())
    {
        return false;
    }


    TiXmlDocument doc;
    if(!doc.LoadFile(fileName.c_str()))
    {
        return false;
    }

    if (!sParentPath.empty())
    {

    }

    // top level node

    TiXmlElement* root = doc.FirstChildElement("Config");
    if (0 == root)
    {
        return false;
    }

    // clear old
    ClearConfigInfos();


    // new node
    TiXmlElement* ui = root->FirstChildElement("UIConfig");
    if (0 != ui)
    {
        TiXmlElement* tree = ui->FirstChildElement("TestTree");
        if(0 != tree)
        {
            int val=0;
            tree->QueryIntAttribute("columnCount",&val);
            m_nTreeColumnCount=val;

            tree->QueryIntAttribute("textType",&val);
            m_nTextType=val;
        }     
    }

    // base info node
    TiXmlElement* base = root->FirstChildElement("BaseConfig");
    if( 0 != base )
    {
//        int pid = -1;
//        base->QueryIntAttribute("projectID",&pid);
//        m_nProjectID = pid;
//        m_sProjectName = base->Attribute("project");

        for (TiXmlElement* baseGrp = base->FirstChildElement("BaseGroup"); baseGrp; baseGrp = baseGrp->NextSiblingElement())
        {
            if ( 0 !=  baseGrp)
            {
                int id = -1;
                baseGrp->QueryIntAttribute("bid",&id);
                std::string name = baseGrp->Attribute("name");

                CItemsGroup* pGrp = new CItemsGroup(id, name);
                m_vCustomGroups.push_back(pGrp);

                if(!LoadItemInfoNodes(baseGrp, *(pGrp->GetGroupItems()), "Item"))
                {
                    return false;
                }
            }
        }
    }

    // test tag groups
    TiXmlElement* ttags = root->FirstChildElement("TestTagConfig");
    if (0 == ttags)
    {
        return false;
    }


    CManageItemStrategy * pStrategy = CManageItemStrategy::GetInstance();

    int nItemAllCount = 0;
    for (TiXmlElement* grp = ttags->FirstChildElement("TestTagGroup"); grp; grp = grp->NextSiblingElement())
    {
        if( 0 == ttags )
        {
            continue;
        }

        int port = -1;
        grp->QueryIntAttribute("id",&port);
        std::string name = grp->Attribute("name");


        // check if id repeated?
        if( NULL != pStrategy )
        {
            CTestTagGroup * pItemChk = pStrategy->IsHaveItemID<CTestTagGroup, std::vector<CTestTagGroup*>::iterator>(port,&m_vTestTagGroups );
            if(NULL != pItemChk)
            {
                return false;
            }
        }



        int nEditType = -1;
        grp->QueryIntAttribute("editType",&nEditType);

        CTestTagGroup* cgrp = new CTestTagGroup(port, name);
        cgrp->SetTestTagGroupEditType(nEditType);

        int nval = -1;
        if( 0 == grp->QueryIntAttribute("isShow",&nval))
        {
            cgrp->SetItemShowFlag(nval==1);
        }

        m_vTestTagGroups.push_back(cgrp);

        vector<CItemInfo*>* pItems = cgrp->GetGroupItems();

        if(!LoadItemInfoNodes(grp,*pItems, "Item"))
        {
            return false;
        }
        nItemAllCount += (*pItems).size();
    }

    m_nAllTestTagItemCount = nItemAllCount;

    return true;
}

///
/// \brief CLogConfig::LoadItemInfoNodes
/// \param pnt
/// \param vData
/// \param itemName
/// \return
/// load standard item for tag and base
///
bool CLogConfig::LoadItemInfoNodes( TiXmlElement * const pnt, vector<CItemInfo*> & vData , const std::string & itemName)
{
    CManageItemStrategy * pStrategy = CManageItemStrategy::GetInstance();

    for (TiXmlElement* itm = pnt->FirstChildElement(itemName); itm; itm = itm->NextSiblingElement())
    {
        int id = -1;
        itm->QueryIntAttribute("id",&id);
        std::string name = itm->Attribute("name");

        // check if id repeated?
        if( NULL != pStrategy )
        {
            CItemInfo * pItemChk = pStrategy->IsHaveItemID<CItemInfo, std::vector<CItemInfo*>::iterator>(id,&vData );
            if(NULL != pItemChk)
            {
                return false;
            }
        }


        CItemInfo* citem = new CItemInfo(id, name);
        vData.push_back(citem);

        int nval = -1;
        if( 0 == itm->QueryIntAttribute("isShow",&nval))
        {
            citem->SetItemShowFlag(nval==1);
        }

        nval = -1;
        if( 0 == itm->QueryIntAttribute("editType",&nval))
        {
            citem->SetItemEditFlag(EIssueEditType(nval));

            nval = -1;
            if(0 == itm->QueryIntAttribute("bid",&nval))
            {
                citem->SetItemBaseInfoFlag(EBaseInfoType(nval));
            }
        }
    }

    return true;
}

///
/// \brief CLogConfig::LoadBaseConfigNode
/// \param base
/// \param vData
/// \param nodeName
/// \return
/// load base infos
///
bool CLogConfig::LoadBaseConfigNode(TiXmlElement * const base, vector<CItemInfo*> & vData, const std::string & nodeName )
{
    TiXmlElement* child = base->FirstChildElement(nodeName);
    if ( 0 !=  child)
    {
        if(!LoadItemInfoNodes(child, vData, "Item"))
        {
            return false;
        }
    }
    return true;
}

///
/// \brief CLogConfig::DeleteAllChilds
/// \param pnt
/// \return
/// iterator delete all child nodes
///
bool CLogConfig::DeleteAllChilds(TiXmlElement * const pnt)
{
    for (TiXmlElement* itm = pnt->FirstChildElement(); itm; )
    {
        TiXmlElement* itmN = itm->NextSiblingElement();

        pnt->RemoveChild(itm);
        itm = itmN;
    }
    return true;
}

///
/// \brief CLogConfig::SaveItemInfoNodes
/// \param pnt
/// \param vData
/// \param itemName
/// \return
/// save standard nodes
///
bool CLogConfig::SaveItemInfoNodes(TiXmlElement *  const pnt, vector<CItemInfo*> & vData , const std::string & itemName)
{
    //    vData.push_back(new CItemInfo(1,"A"));
    for (std::vector<DEBUGTOOL::CItemInfo*>::iterator it = vData.begin(); it != vData.end(); ++it)
    {
        std::string name = (*it)->GetItemName();
        int ID = (*it)->GetItemID();
        bool bShow = (*it)->GetItemShowFlag();
        EIssueEditType eEdit = (*it)->GetItemEditFlag();
        EBaseInfoType eBid = (*it)->GetItemBaseInfoFlag();

        TiXmlElement* item = new TiXmlElement(itemName);
        pnt->LinkEndChild( item );

        item->SetAttribute("id",(int)ID);
        item->SetAttribute("name",name);
        item->SetAttribute("isShow", bShow ? 1 : 0);

        if( EIT_DEFAULT_NO != eEdit)
        {
            item->SetAttribute("editType", (int)eEdit);
        }

        if( EBT_DEFAULT_NO != eBid)
        {
            item->SetAttribute("bid", (int)eBid);
        }
    }
    return true;
}

///
/// \brief CLogConfig::SaveBaseConfigNode
/// \param base
/// \param vData
/// \param nodeName
/// \return
/// save base nodes
///
bool CLogConfig::SaveBaseConfigNode(TiXmlElement * const base,  vector<CItemInfo*> & vData, const std::string & nodeName)
{
    TiXmlElement* child = base->FirstChildElement(nodeName);
    if (0 == child)
    {
        child = new TiXmlElement(nodeName);
        base->LinkEndChild( child );
    }

    if(!DeleteAllChilds(child))
    {
        return false;
    }

    if(!SaveItemInfoNodes(child, vData, "Item"))
    {
        return false;
    }

    return true;
}

///
/// \brief CLogConfig::SaveConfigFile
/// \param eType
/// \return
///  save current config-xxx.xml file
///
bool CLogConfig::SaveConfigFile(const EConfigFileNodeType& eType)
{
    TiXmlDocument doc;

//    int ver = 1;
    TiXmlElement* root = 0;
    std::string fileName = GetConfigFile();
    if(doc.LoadFile(fileName))
    {
        root = doc.FirstChildElement("Config");
    }

    // top level node

    TiXmlComment comment;
    if( 0 == root )
    {
        doc.Clear();
				
        TiXmlDeclaration decl("1.0","UTF-8","false");
        doc.InsertEndChild(decl);

        comment.SetValue("Test Tag Configuration");
        doc.InsertEndChild(comment);
        root = new TiXmlElement("Config");
        doc.LinkEndChild( root );
    }


    if(eType == CNT_Project_Base || eType == CNT_DEFAULT)
    {
        // new node
        TiXmlElement* ui = root->FirstChildElement("UIConfig");
        if (0 == ui)
        {
            ui = new TiXmlElement("UIConfig");
            root->LinkEndChild(ui);

            comment.SetValue("columnCount : set column count for test tag show.");
            ui->InsertEndChild(comment);
        }
        TiXmlElement* tree = ui->FirstChildElement("TestTree");
        if(0 == tree)
        {
            tree = new TiXmlElement("TestTree");
            ui->LinkEndChild(tree);
        }
        tree->SetAttribute("columnCount",(int)m_nTreeColumnCount);
    //    tree->SetAttribute("textType",m_nTextType);
    }


    if(eType == CNT_Tag_Base_Infos || eType == CNT_DEFAULT)
    {
        // bast infos node
        TiXmlElement* base = root->FirstChildElement("BaseConfig");
        if( 0 == base )
        {
            base = new TiXmlElement("BaseConfig");
            //        root->InsertBeforeChild(ttags,base);
            root->LinkEndChild( base );

            // add comment
            comment.SetValue("Base elements for selection");
            base->InsertEndChild(comment);

            comment.SetValue("isShow: 0 hide, 1 show");
            base->InsertEndChild(comment);
        }

        base->SetAttribute("project",GetProjectName());
        base->SetAttribute("projectID",GetProjectID());

        SaveItemsGroup<CItemsGroup,vector<CItemsGroup*>::iterator>(base, m_vCustomGroups);
    }

    if(eType == CNT_Tag_Items || eType == CNT_DEFAULT)
    {
        // test tags node
        TiXmlElement* ttags = root->FirstChildElement("TestTagConfig");
        if( 0 == ttags )
        {
            ttags = new TiXmlElement("TestTagConfig");
            root->LinkEndChild( ttags );

            comment.SetValue("All test tags for selection");
            ttags->InsertEndChild(comment);

            // add comment
            {
                comment.SetValue("editType: 0 no eidt, 1 add edit line, 2 add combobox group, 3 add_edit_line + issue_supply_edit; 11 edit in line, 12 combox in line-swc");
                ttags->InsertEndChild(comment);
                comment.SetValue("isShow: 0 hide, 1 show");
                ttags->InsertEndChild(comment);
            }
        }
        SaveItemsGroup<CTestTagGroup,vector<CTestTagGroup*>::iterator>(ttags, m_vTestTagGroups);
    }

    doc.SaveFile();
    return true;
}

///
/// \brief CLogConfig::SaveItemsGroup
/// \param ttags
/// \param vGroup
/// \return
/// template, save standard item group
///
template<typename TGroup, typename TIterator>
bool CLogConfig::SaveItemsGroup(TiXmlElement*& ttags, std::vector<TGroup*> &  vGroup)
{
    DeleteAllChilds(ttags);

    for (TIterator it = vGroup.begin(); it != vGroup.end(); ++it)
    {
        std::string name = (*it)->GetItemName();
        int nGrpID = (*it)->GetItemID();
        bool bShow = (*it)->GetItemShowFlag();
        std::vector<DEBUGTOOL::CItemInfo*> * pItems = (*it)->GetGroupItems();

        std::string grp_name = "BaseGroup";
        std::string bid_name = "bid";

        CTestTagGroup * pPointTest = dynamic_cast<CTestTagGroup*>(*it);
        if( NULL != pPointTest)
        {
            grp_name = "TestTagGroup";
            bid_name = "id";
        }

        TiXmlElement* grp = new TiXmlElement(grp_name);
        ttags->LinkEndChild( grp );

        grp->SetAttribute(bid_name.c_str(),	nGrpID);
        grp->SetAttribute("name",name);
        grp->SetAttribute("isShow",bShow ? 1 : 0);

        if( NULL != pPointTest)
        {
            EIssueEditType nEditType = pPointTest->GetTestTagGroupEditType();
            grp->SetAttribute("editType",(int)nEditType);
        }

        if(!SaveItemInfoNodes(grp, (*pItems), "Item"))
        {
            return false;
        }
    }
}

///
/// \brief CLogConfig::SaveConfigFile_SingleInfo
/// \return
/// backup fun
///
bool CLogConfig::SaveConfigFile_SingleInfo()
{
    TiXmlDocument doc;

    TiXmlElement* root = 0;
    std::string fileName = GetConfigFile();
    if(doc.LoadFile(fileName))
    {
        root = doc.FirstChildElement("Config");
    }

    if( 0 == root )
    {
        return false;
    }

    // new node
    TiXmlElement* ui = root->FirstChildElement("UIConfig");
    if (0 == ui)
    {
        return false;
    }

    TiXmlElement* tree = ui->FirstChildElement("TestTree");
    if(0 == tree)
    {
        return false;
    }
    tree->SetAttribute("columnCount",(int)m_nTreeColumnCount);


    doc.SaveFile();
    return true;
}


//void CLogConfig::SetLcmLogFileName(const std::string & fileName)
//{
//    if( fileName == m_sLcmLogFileName)
//    {
//        return;
//    }
//    m_sLcmLogFileName = fileName;
//    m_bLcmLogFileUpdated = true;
//}

///
/// \brief CLogConfig::SetIssueLogCompressed
/// \param bCompressed
/// \return
/// check and save
bool CLogConfig::SetIssueLogCompressed(const bool& bCompressed)
{
    if(m_bIssueLogCompressed == bCompressed)
    {
        return false;
    }

    m_bIssueLogCompressed = bCompressed;
    return true;
}

///
/// \brief CLogConfig::SetNoIssueLogCompressed
/// \param bCompressed
/// \return
///
bool CLogConfig::SetNoIssueLogCompressed(const bool& bCompressed)
{
    if(m_bNoIssueLogCompressed == bCompressed)
    {
        return false;
    }

    m_bNoIssueLogCompressed = bCompressed;
    return true;
}

///
/// \brief CLogConfig::SetCompressedCPUCount
/// \param cnt
/// \return
/// check and save
bool CLogConfig::SetCompressedCPUCount(const short& cnt)
{
    if(m_nCompressionCPUCount == cnt)
    {
        return false;
    }
    m_nCompressionCPUCount = cnt;
    return true;
}

///
/// \brief CLogConfig::SetTestBaseRecordInfos
/// \param eType
/// \param val
///
bool CLogConfig::SetTestBaseRecordInfos(const ETestBaseInfos& eType, const std::string& val)
{
    bool bUpdated = false;
    switch(eType)
    {
    //case  TBI_Project:
    case TBI_SW_Version_No:
        bUpdated = (m_sSWVersionNo != val);
        m_sSWVersionNo = val;
        break;
    case TBI_Vehicle_No:
        bUpdated = (m_sVehichleNo != val);
        m_sVehichleNo = val;
        break;
    case TBI_Tester:
        bUpdated = (m_sTester != val);
        m_sTester = val;
        break;
    case TBI_Disk_No:
        bUpdated = (m_sDiskNo != val);
        m_sDiskNo = val;
        break;
    }
    return bUpdated;
}

///
/// \brief CLogConfig::GetTestBaseRecordInfos
/// \param eType
/// \return
/// for default saved show
///
std::string CLogConfig::GetTestBaseRecordInfos(const ETestBaseInfos& eType)
{
    std::string sVal="";
    switch(eType)
    {
    //case  TBI_Project:
    case TBI_SW_Version_No:
        sVal = m_sSWVersionNo;
        break;
    case TBI_Vehicle_No:
        sVal = m_sVehichleNo;
        break;
    case TBI_Tester:
        sVal = m_sTester;
        break;
    case TBI_Disk_No:
        sVal = m_sDiskNo;
        break;
    }
    return sVal;
}

///
/// \brief CLogConfig::SetTestBaseInfo
/// \param info
/// set base info into a string, from UI setting
///
void CLogConfig::SetTestBaseInfo(const std::string & info)
{
    if( info == m_sBaseInfo)
    {
        return;
    }
    m_sBaseInfo = info;
    m_bBaseInfoUpdated = true;

    SaveProjectFile(PNT_TestBaseInfo_Default);
}


///
/// \brief CLogConfig::SetTestTagFileParentPath
/// \param path
/// set operation history file path-name
///
void CLogConfig::SetTestTagFileParentPath(const std::string & path)
{
    if (path == m_sTestTagFileParentPath)
    {
        return ;
    }
    m_sTestTagFileParentPath = path;
    m_bTestTagFilePathUpdated = true;
}

///
/// \brief CLogConfig::GetTagOperateFileName
/// \param filePath
/// \return
/// get operation history file path-name
///
bool CLogConfig::GetTagOperateFileName(  std::string & filePath)
{
    if (m_sTestTagOperateFileName.empty())
    {
        return false;
    }

    std::string  sParentPath = m_sTestTagFileParentPath;
    if(m_sTestTagFileParentPath.empty())
    {
        sParentPath = ".";
    }

    filePath = sParentPath + "/" + m_sTestTagOperateFileName;

    return true;
}

///
/// \brief CLogConfig::TestTagFile_RemoveOld
/// \param filePath
/// \param nRemovedCnt
/// 按文件大小，定长删除旧的字段
///
void CLogConfig::TestTagFile_RemoveOld(const std::string& filePath, int& nRemovedCnt)
{
    if(m_nOldFileLineCount < MAX_TEST_TAG_FILE_COUNT)
    {
        return;
    }
    nRemovedCnt = m_nOldFileLineCount-(MAX_TEST_TAG_FILE_COUNT-TEST_TAG_FILE_REMOVE_COUNT);

    // read
    std::ifstream file_in;
    file_in.open(filePath.c_str(),  std::ios::in);

    std::string temp, sNext="";
    int nLineCnt = 0;
    while(std::getline(file_in,temp))
    {
        nLineCnt ++;
        if(nLineCnt > nRemovedCnt)
        {
            if(!sNext.empty())
            {
                sNext += "\n";
            }
            sNext += temp ;
        }
    }
    file_in.close();

    // write
    std::ofstream file_out;
    file_out.open(filePath.c_str(),  std::ios::out);
    file_out.flush();
    file_out << sNext << std::endl;
    file_out.close();

    m_nOldFileLineCount -= nRemovedCnt;
}

///
/// \brief CLogConfig::GetTestTagFileCount
/// \param filePath
/// \return
/// 获取tag日志文件行数
///
bool CLogConfig::GetTestTagFileCount(const std::string& filePath)
{
    if(0 > m_nOldFileLineCount )
    {
        int nLineCnt = 0;

        std::ifstream file;
        file.open(filePath.c_str(), std::ios::in);
        std::string temp;
        while(std::getline(file,temp))
        {
            nLineCnt ++;
        }
        file.close();
        m_nOldFileLineCount = nLineCnt;
    }

    return true;
}

///
/// \brief CLogConfig::SaveTestTagOperateFile
/// \param time
/// \param pinfo
/// \param bChecked
/// \param sTag
/// \param nLineIndex
/// \param sFile
/// \param nRemovedCnt
/// \return
/// 新增日志时的文件保存，新插入一行。 根据界面操作，实时后台更新，test tag 日志文件
///
bool CLogConfig::SaveTestTagOperateFile(const std::string & time, const int& parentID, CItemInfo * const pinfo, const bool & bChecked, const std::string & sTag , int & nLineIndex, const std::string& sFile, int& nRemovedCnt)
{
    std::string filePath;
    if(!GetTagOperateFileName(filePath))
    {
        return false;
    }

    if( ! pinfo )
    {
        return false;
    }

    std::string tag = pinfo->GetItemName();
    if(!sTag.empty())
    {
        tag = sTag;
    }

    static bool bCreateFile = false;

    if(bCreateFile)
    {
        // file already created
        GetTestTagFileCount(filePath);

        nRemovedCnt = 0;
        if(m_nOldFileLineCount >= MAX_TEST_TAG_FILE_COUNT-1)
        {
            TestTagFile_RemoveOld(filePath, nRemovedCnt);
        }
    }

    std::ofstream file;
    if(!bCreateFile)
    {
        int nSufIndex = filePath.find_last_of(".");
        std::string bufFile = filePath;
        bufFile.insert(nSufIndex,".old");

        // create a new file, last file append to **.old
        std::string cmd = "if [ -f " + filePath + " ];then cat " + filePath + " >> " + bufFile + "; fi";
        int nres = system(cmd.c_str());

        // process old file, delelte old lines
//        cmd="cnt=2; ((b=cnt-1))";
        cmd = "if [ -f " + bufFile + " ];then cnt=`wc -l " + bufFile + " | awk '{print $1}'`; dcnt=$(($cnt-2000)); if [ $cnt -gt 2000 ];then sed -i 1,\"$dcnt\"d " + bufFile + "; fi; fi";
        nres = system(cmd.c_str());


        m_nOldFileLineCount = 0;

        file.open(filePath.c_str());
        bCreateFile = true;
    }
    else
    {
        file.open(filePath.c_str() ,std::ios::app);  // app
    }


    // save base info
    if (m_bBaseInfoUpdated)
    {
        std::string baseInfo = "[test info start]\n" + m_sBaseInfo + "\n[test info end]";

        file << baseInfo << std::endl ;
        m_nOldFileLineCount += 7;

        m_bBaseInfoUpdated = false;
    }
    if (m_bTestTagFilePathUpdated)
    {
        std::string lcmFile = "Log Path: " + m_sTestTagFileParentPath ;
        file << lcmFile << std::endl ;
        m_nOldFileLineCount ++;

        m_bTestTagFilePathUpdated = false;
    }

    if(bChecked)
    {
        int id = pinfo->GetItemID();
        int projectID = GetProjectID();

//        file << setw(7) << setfill('0') << internal << -1 << std::endl;
        file << time <<";;"<< setw(3) << setfill('0') << internal  <<  projectID  << ";" << setw(3)<<  parentID  << ";" << setw(3) << id << "; " << tag << "; "  << sFile << std::endl ;
        m_nOldFileLineCount ++;
        nLineIndex = m_nOldFileLineCount;
    }

    file.close();
    return true;
}

///
/// \brief CLogConfig::SaveBaseInfoText
/// \return
/// only save base info, not use
///
bool CLogConfig::SaveBaseInfoText()
{
    std::string filePath;
    if(!GetTagOperateFileName(filePath))
    {
        return false;
    }
    if(m_sBaseInfo.empty())
    {
        return false;
    }

    if(!m_bBaseInfoUpdated)
    {
        return true;
    }

    std::ofstream file;

    if(0 == m_nOldFileLineCount)
    {
        file.open(filePath.c_str(),std::ios::out | std::ios::trunc );
    }
    else
    {
        file.open(filePath.c_str(),std::ios::out );
    }

    // save base info

    std::string baseInfo = "[test info start]\n" + m_sBaseInfo + "\n[test info end]";
    if(0 == m_nOldFileLineCount)
    {
        file << baseInfo << std::endl ;

        m_nOldFileLineCount += 7;
    }
    else
    {
        file.seekp(0,ios::beg);
        file << baseInfo << std::endl ;
    }

    m_bBaseInfoUpdated = false;


    file.close();
    return true;

}


///
/// \brief CLogConfig::GetUpdatedString
/// \param sCurLine
/// \param text
/// \param nType
/// \return
/// 获取更新后字段
///
bool CLogConfig::GetUpdatedString(std::string &sCurLine, const std::string & text_org, const int& nType)
{
    ETestTagEditColumns eType =(ETestTagEditColumns)nType;
    std::string text = text_org;
    text.insert(0," ");

    if(eType == TEC_End_Time)
    {
        sCurLine.insert(sCurLine.find_first_of(";")+1,text);
    }
    else if(eType == TEC_Issue )
    {
        std::string sTemp = sCurLine;
        size_t pos = 0, curpos = 0, nBeg, nEnd;
        int nIdx = 0;
        while(pos < sTemp.length())
        {
            pos = sTemp.find(";",curpos);
            if(pos != string::npos)
            {
                nIdx++;
                curpos = pos + 1;
            }
            if( 5 == nIdx)
            {
                nBeg = pos;
            }
            else if(6 == nIdx)
            {
                nEnd = pos;
                break;
            }
        }

        sCurLine.replace(nBeg+1, nEnd-nBeg-1,text);
    }
    else if(eType == TEC_Log_File_Name)
    {
        int nlen = sCurLine.length()-sCurLine.find_last_of(";");
        sCurLine.replace(sCurLine.find_last_of(";")+1, nlen,text);
    }
    else
    {
        return false;
    }
    return true;

}

///
/// \brief CLogConfig::UpdateTestTagText
/// \param nLineIndex
/// \param text
/// \param nType
/// \param text_2
/// \param nType_2
/// \return
/// 根据UI界面上二次编辑功能，查询修改tag日志文件
///
bool CLogConfig::UpdateTestTagText(const int & nLineIndex,const std::string & text,  const int& nType, const std::string & text_2, const int& nType_2)
{
    if(0 >= nLineIndex)
    {
        return false;
    }
    if(nType >= TEC_End || nType < 0)
    {
        return false;
    }
    if(text.empty())
    {
        return false;
    }

    if( !text_2.empty())
    {
        if(nType_2 >= TEC_End || nType_2 < 0 )
        {
            return false;
        }
    }


    std::string filePath;
    if(!GetTagOperateFileName(filePath))
    {
        return false;
    }

    // read
    std::ifstream file_in;
    file_in.open(filePath.c_str(),  std::ios::in);

    std::string temp, sCurLine, sNext="", sBefore="";
    int nLineCnt = 0;
    while(std::getline(file_in,temp))
    {
        nLineCnt ++;
        if(nLineCnt == nLineIndex)
        {
            sCurLine = temp;
        }
        else if(nLineCnt > nLineIndex)
        {
            if(!sNext.empty())
            {
                sNext += "\n";
            }
            sNext += temp ;
        }
        else
        {
            if(!sBefore.empty())
            {
                sBefore += "\n";
            }
            sBefore += temp ;
        }
    }
    file_in.close();


    if(!GetUpdatedString(sCurLine, text,nType))
    {
        return false;
    }

    if( !text_2.empty())
    {
        if(!GetUpdatedString(sCurLine,text_2, nType_2))
        {
            return false;
        }
    }


    if(!sBefore.empty())
    {
        sCurLine = sBefore + "\n" + sCurLine;
    }
    if(!sNext.empty())
    {
        sCurLine += "\n" + sNext;
    }

    // write
    std::ofstream file_out;
    file_out.open(filePath.c_str(),  std::ios::out);
//    file_out.seekp(-nOld, ios::end);
//    file_out.write(sCurLine.c_str(),sCurLine.length());
    file_out.flush();
    file_out << sCurLine << std::endl;
    file_out.close();

    return true;
}

//bool TdrawSvg::Save2File(char* _pcFullFileName)
//{
//    // http://blog.csdn.net/u011314012/article/details/49861363
//    // https://stackoverflow.com/questions/4780507/create-utf-8-file-in-qt

//    QString str = FpOwnerWebView->page()->currentFrame()->toHtml();
//    //QString unicodeString = "Some Unicode string";
//    QFile fileOut("C:\\zzz.svg");
//    //if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Truncate))
//    if (! fileOut.open(QFile::WriteOnly | QFile::Truncate))
//        return false;

//    QTextStream streamFileOut(&fileOut);
//    streamFileOut.setCodec("UTF-8");
//    streamFileOut << str;
//    streamFileOut.flush();

//    // ZC: 查到说，下面的参数是true的话 就是保存成"UTF-8 + BOM"，false的话 就是保存成"UTF-8"
//    //  但是，我尝试了一下，都是"UTF-8"... 不知为啥...
//    streamFileOut.setGenerateByteOrderMark(true);

//    fileOut.close();
//}

#include <sys/stat.h>

bool CLogConfig::check_file_exists (const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}



///----------------------------------------------------------------------


///
///  for player
///
/// \brief CTagInfo::CTagInfo
///
CTagInfo::CTagInfo()
    : CAbstractItemInfo()
    , m_sStartTime("")
    , m_sEndTime("")
    , m_sLogNames("")
    , m_nProjectID(-1)
    , m_nIssueTypeID(-1)
{

}

CTagInfo::~CTagInfo()
{

}

void CTagInfo::SetStartTime(const std::string& tm)
{
    m_sStartTime = tm;
}
const std::string CTagInfo::GetStartTime()const
{
    return m_sStartTime;
}

void CTagInfo::SetEndTime(const std::string& tm)
{
    m_sEndTime = tm;
}

const std::string CTagInfo::GetEndTime()const
{
    return m_sEndTime;
}

void CTagInfo::SetLogNames(const std::string& s)
{
    m_sLogNames = s;
}

const std::string CTagInfo::GetLogNames()const
{
    return m_sLogNames;
}

void CTagInfo::SetProjectID(const int& id)
{
    m_nProjectID = id;
}

const int CTagInfo::GetProjectID()const
{
    return m_nProjectID;
}

void CTagInfo::SetIssueTypeID(const int& id )
{
    m_nIssueTypeID = id;
}

const int CTagInfo::GetIssueTypeID()const
{
    return m_nIssueTypeID;
}

std::string CTagInfo::GetTagInfo(const int & type) const
{
    std::string val="";
    ETestTagEditColumns eType = (ETestTagEditColumns)type;
    switch(eType)
    {
    case  TEC_Start_Time:
        val = m_sStartTime;
        break;
    case TEC_End_Time:
        val = m_sEndTime;
        break;
    case TEC_Issue:
        val = GetItemName();
        break;
    case TEC_Log_File_Name:
        val = m_sLogNames;
        break;
//    case TEC_Log_File_Path:
    default:
        val="";
        break;
    }
    return val;
}

///
/// \brief CPlayerConfig::CPlayerConfig
///

CPlayConfig::CPlayConfig()
    : m_sTestTagFile("")
    , m_sBaseInfo("")
{

}
CPlayConfig::~CPlayConfig()
{
    ClearTagGroup();
}

void CPlayConfig::ClearTagGroup()
{
    for(std::vector<CTagInfo*>::iterator it = m_vTagInfos.begin(); it != m_vTagInfos.end(); ++it)
    {
        if(*it)
        {
            delete (*it);
            (*it) = NULL;
        }
    }
    m_vTagInfos.clear();
    m_vTagInfos.shrink_to_fit();
}

///
/// \brief CPlayConfig::GetLogFileNames
/// \param parentPath
/// \param vFiles
/// \return
/// read directory, get all log compress file
///
bool CPlayConfig::GetLogFileNames(const std::string& parentPath, vector<std::string>& vFiles)
{
    DIR *dir;
    struct dirent *ptr;

    dir = opendir(parentPath.c_str());
    if(NULL == dir)
    {
        return false;
    }


    vector<std::string> vAllFiles;

    while(NULL != (ptr = readdir(dir)))
    {
        if(ptr->d_name[0] == '.')
        {
            continue;
        }

        if(ptr->d_type != 8) //file: 8, link file 10, subdir 4
        {
            continue;
        }
        std::string fileName = ptr->d_name;

        if(fileName == DEBUGTOOL::TEST_TAG_FILE_NAME)
        {
            m_sTestTagFile = parentPath + "/" + fileName;
            continue;
        }

        int nE = fileName.find_first_of(".");
        if(nE < 0)
        {
            continue;
        }


        vAllFiles.push_back(fileName);
    }
    closedir(dir);

    if(vAllFiles.size() > 2)
    {
        // asc
        sort(vAllFiles.begin(),vAllFiles.end());
    }

    vFiles.clear();

    std::string fileName;
    bool bfind  = false;
    for(vector<std::string>::iterator it = vAllFiles.begin(); it!= vAllFiles.end(); ++it)
    {
        fileName = (*it);

        bfind  = false;

        // check suf:  1 .tar.xz  2 .log
        if(CheckLogFileFormat(fileName,2))
        {
            bfind = true;
        }
        else
        {
            if(CheckLogFileFormat(fileName,1))
            {
                // log ralated .tar.xz not exist, record this fileName
                if(!CheckLogExist(fileName,vFiles))
                {
                    bfind = true;
                }
            }
        }

        if(bfind)
        {
            vFiles.push_back(fileName);
        }
    }

    vAllFiles.clear();


//    while(NULL != (ptr = readdir(dir)))
//    {
//        if(ptr->d_name[0] == '.')
//        {
//            continue;
//        }

//        if(ptr->d_type != 8) //file: 8, link file 10, subdir 4
//        {
//            continue;
//        }
//        std::string fileName = ptr->d_name;

//        if(fileName == DEBUGTOOL::TEST_TAG_FILE_NAME)
//        {
//            m_sTestTagFile = parentPath + "/" + fileName;
//            continue;
//        }

//        int nE = fileName.find_first_of(".");
//        if(nE < 0)
//        {
//            continue;
//        }

//        bool bfind  = false;
//        // check suf:  1 .tar.xz  2 .log
//        if(CheckLogFileFormat(fileName,1))
//        {
//            bfind = true;
//        }
//        else
//        {
//            if(CheckLogFileFormat(fileName,2))
//            {
//                // log ralated .tar.xz not exist, record this fileName
//                if(!CheckLogExist(fileName,vFiles))
//                {
//                    bfind = true;
//                }
//            }
//        }

//        if(bfind)
//        {
//            vFiles.push_back(fileName);
//        }
//    }

//    closedir(dir);

    if(vFiles.size() > 2)
    {
        // desc
//        sort(vFiles.rbegin(),vFiles.rend());
        // asc
        sort(vFiles.begin(),vFiles.end());
    }

    return true;
}

///
/// \brief CPlayConfig::CheckLogFileFormat
/// \param fileName
/// \param type
/// \return
/// check log file format is ok or not?
///
bool CPlayConfig::CheckLogFileFormat(const std::string fileName, const short& type)
{
    std::string sufStd = "";
    if (1 == type)
    {
        sufStd = DEBUGTOOL::COMPRESS_SUFFIX;
    }
    else if(2 == type)
    {
        sufStd = DEBUGTOOL::LOG_SUFFIX;
    }
    else
    {
        return false;
    }

    int nSufLen = sufStd.length();

    int nE = fileName.find_first_of(".");
    if(nE < 0)
    {
        return false;
    }

    bool bfind  = false;
    // check suf is .tar.xz
    std::string suf = fileName.substr(nE,nSufLen);
    if(suf == sufStd)
    {
        int nSuf = fileName.length()-nSufLen;
        if(nE == nSuf)
        {
            bfind = true;
        }
    }

    return bfind;
}

///
/// \brief CPlayConfig::CheckLogExist
/// \param fileName
/// \param vFiles
/// \return
/// check log name-main exist or not?
/// return true: exist
///        false: not exist
///
bool CPlayConfig::CheckLogExist(const std::string& fileName, vector<std::string>& vFiles)
{
    int nE = fileName.find_first_of(".");
    if(nE < 0)
    {
        return false;
    }

    std::string sFileFst = fileName.substr(0,nE);

    for(vector<std::string>::iterator it = vFiles.begin(); it != vFiles.end(); ++it)
    {
        std::string name = (*it);
        int nSuf = name.find_first_of(".");
        if(nSuf < 0)
        {
            continue;
        }
        std::string vNameFst = name.substr(0,nSuf);

        if(sFileFst == vNameFst)
        {
            return true;
        }
    }

    return false;
}

///
/// \brief CPlayConfig::SetTestTagFileName
/// \param file
/// \return
/// set tag file for player
///
bool CPlayConfig::SetTestTagFileName(const std::string& file)
{
    m_sTestTagFile = file;

    return OpenTestTagsFile();
}

std::string CPlayConfig::GetTestTagFileName() const
{
    return m_sTestTagFile;
}

void CPlayConfig::StringTrim(  std::string& str)
{
    if(str.empty())
    {
        return ;
    }
    str.erase(0,str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
}

///
/// \brief CPlayConfig::OpenTestTagsFile
/// \return
/// open test tag file in player
///
bool CPlayConfig::OpenTestTagsFile()
{
    if(m_sTestTagFile.empty())
    {
        return false;
    }

    ClearTagGroup();

    // read
    std::ifstream file_in;
    file_in.open(m_sTestTagFile.c_str(),  std::ios::in);

    static int issue_section_cnt = 7;
    std::string temp,  sBase="";
//    int nLineCnt = 0;
    bool bBase = false;
    while(std::getline(file_in,temp))
    {
        if(temp == "[test info start]")
        {
            bBase = true;
            continue;
        }
        else if(temp == "[test info end]")
        {
            bBase = false;
            continue;
        }
        else if(temp.empty())
        {
            continue;
        }
        else if(temp.substr(0,8) == "Log Path")
        {
            continue;
        }

        if(bBase)
        {
            if(!sBase.empty())
            {
                sBase += "  ";
            }
            if(temp.find("Project") != string::npos)
            {
                if(sBase.find("Project") != string::npos)
                {
                    sBase = "";
                }
            }
            sBase += temp;
            continue;
        }

        std::string tagVals[issue_section_cnt];
        std::string sTemp = temp, strName;
        int nTCnt = 0;
        size_t nLastPos = 0;
        while(false == sTemp.empty() )
        {
            size_t npos = sTemp.find_first_of(";");
            if(npos == string::npos)
            {
                if(nTCnt == issue_section_cnt-1)
                {
                    tagVals[nTCnt] = sTemp;
                    sTemp = "";
                    nTCnt ++;
                    break;
                }
                else
                {
                    break;
                }
            }

            strName = sTemp.substr(0,npos);
            tagVals[nTCnt] = strName;

            sTemp = sTemp.substr(npos+1,sTemp.length()-npos);
            nLastPos = npos;
            nTCnt ++;
        }
        if(issue_section_cnt !=  nTCnt)
        {
            continue;
        }
        for(int i = 0; i<nTCnt ; i++)
        {
            StringTrim(tagVals[i]);
        }


        int idx = 0;
        CTagInfo * info = new CTagInfo();
        info->SetStartTime(tagVals[idx++]);
        info->SetEndTime(tagVals[idx++]);
        info->SetProjectID(std::atoi(tagVals[idx++].c_str()));
        info->SetIssueTypeID(std::atoi(tagVals[idx++].c_str()));
        info->SetItemID(std::atoi(tagVals[idx++].c_str()));
        info->SetItemName(tagVals[idx++]);
        info->SetLogNames(tagVals[idx++]);

        m_vTagInfos.push_back(info);

    }
    file_in.close();

    m_sBaseInfo = sBase;

    if(0 == m_vTagInfos.size())
    {
        return false;
    }


    return true;
}

#include "global.hpp"

///
/// \brief CPlayConfig::GetLogTestTags
/// \param sStartTime
/// \param sStopTime
/// \return
/// get all test tags group, between start time and stop time given
///
vector<CTagInfo* >* CPlayConfig::GetLogTestTags(const long int & nLogStartTime, const long int & nLogStopTime)
{
    int nMaxDelay = 5;
    DEBUGTOOL::CLogConfig * pLogConfig = DEBUGTOOL::CLogConfig::GetInstance();
    if(NULL != pLogConfig)
    {
        nMaxDelay =  pLogConfig->GetIssueCheckMaxTimeDelay();
    }

    m_vCurrentLogTagInfos.clear();

    long int nLogStartTimeSec = nLogStartTime * 1e-6;
    long int nLogStopTimeSec = nLogStopTime * 1e-6;

    for(std::vector<CTagInfo*>::iterator it = m_vTagInfos.begin(); it != m_vTagInfos.end(); ++it)
    {
        std::string issue_start_time = (*it)->GetStartTime();
        std::string issue_stop_time = (*it)->GetEndTime();

        long int nIssuStart = ConvertString2GlobalTimeStampInSec(issue_start_time);
        long int nIssuStop = ConvertString2GlobalTimeStampInSec(issue_stop_time);


        bool bfind = false;
        if(nIssuStart >= nLogStartTimeSec && nIssuStart <= nLogStopTimeSec)
        {
            bfind =  true;
        }
        else if(nIssuStop >= nLogStartTimeSec && nIssuStop <= nLogStopTimeSec)
        {
            bfind = true;
        }
        else if(nIssuStart <= nLogStartTimeSec && nIssuStop >= nLogStopTimeSec)
        {
            bfind = true;
        }
        else if(nIssuStart > nLogStopTimeSec  &&nIssuStart - nLogStopTimeSec <= nMaxDelay)
        {
            bfind = true;
        }

        if(bfind)
        {
            m_vCurrentLogTagInfos.push_back(*it);
        }
    }

    return &m_vCurrentLogTagInfos;
}

///
/// \brief CPlayConfig::GetBaseInfo
/// \return
/// get base info in tag-log file,*.txt
///
std::string CPlayConfig::GetBaseInfo() const
{
    return m_sBaseInfo;
}

