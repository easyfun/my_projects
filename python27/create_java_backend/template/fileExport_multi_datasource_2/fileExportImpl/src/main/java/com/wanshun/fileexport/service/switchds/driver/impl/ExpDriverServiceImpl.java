package com.wanshun.fileexport.service.switchds.driver.impl;

import com.wanshun.common.id.IdService;
import com.wanshun.fileexport.cardatamodel.DataSourceKey;
import com.wanshun.fileexport.cardatamodel.TargetDataSource;
import com.wanshun.fileexport.cardatamodel.dao.driver.ExpDriverReadMapper;
import com.wanshun.fileexport.cardatamodel.po.driver.ExpDriverRead;
import com.wanshun.fileexport.service.switchds.driver.ExpDriverService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@TargetDataSource(DataSourceKey.driverSlave)
@Service
public class ExpDriverServiceImpl implements ExpDriverService {

    @Autowired
    private ExpDriverReadMapper expDriverReadMapper;


    @Override
    public String getPhone(Long driverId) {
        int dataNodeId = IdService.getDataNodeIdByUserId(driverId);
        ExpDriverRead po = expDriverReadMapper.selectByPrimaryKey(driverId, dataNodeId);
        return po.getPhone();
    }
}
