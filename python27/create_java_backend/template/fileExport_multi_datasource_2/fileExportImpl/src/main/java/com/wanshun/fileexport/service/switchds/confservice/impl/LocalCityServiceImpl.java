package com.wanshun.fileexport.service.switchds.confservice.impl;

import com.wanshun.fileexport.cardatamodel.DataSourceKey;
import com.wanshun.fileexport.cardatamodel.TargetDataSource;
import com.wanshun.fileexport.cardatamodel.dao.confservice.LocalCityReadMapper;
import com.wanshun.fileexport.cardatamodel.po.confservice.LocalCityRead;
import com.wanshun.fileexport.service.switchds.confservice.LocalCityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@TargetDataSource(DataSourceKey.confServiceSlave)
@Service
public class LocalCityServiceImpl implements LocalCityService {

    @Autowired
    private LocalCityReadMapper localCityReadMapper;

    @Override
    public String getName(Integer code) {
        LocalCityRead po = localCityReadMapper.selectByPrimaryKey(code);
        return po.getName();
    }
}
