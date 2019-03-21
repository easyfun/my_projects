package com.wanshun.poolfile.cardatamodel.dao.driver.common;

import com.wanshun.poolfile.cardatamodel.po.driver.FileExport;

public interface CommonFileExportMapper {
    int deleteByPrimaryKey(Long id);

    int insertSelective(FileExport record);

    FileExport selectByPrimaryKey(Long id);

    int updateByPrimaryKeySelective(FileExport record);
}