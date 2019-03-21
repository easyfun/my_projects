package com.wanshun.fileexport.cardatamodel.dao.confservice.common;

import com.wanshun.fileexport.cardatamodel.po.confservice.LocalCityRead;

public interface CommonLocalCityReadMapper {
    int deleteByPrimaryKey(Integer code);

    int insertSelective(LocalCityRead record);

    LocalCityRead selectByPrimaryKey(Integer code);

    int updateByPrimaryKeySelective(LocalCityRead record);
}