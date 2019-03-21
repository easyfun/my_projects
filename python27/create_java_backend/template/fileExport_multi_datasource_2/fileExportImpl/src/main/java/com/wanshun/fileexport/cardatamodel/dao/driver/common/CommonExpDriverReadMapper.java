package com.wanshun.fileexport.cardatamodel.dao.driver.common;

import com.wanshun.fileexport.cardatamodel.po.driver.ExpDriverRead;
import java.util.List;
import org.apache.ibatis.annotations.Param;

public interface CommonExpDriverReadMapper {
    int deleteByPrimaryKey(@Param("driverId") Long driverId, @Param("dataNodeId") Integer dataNodeId);

    int insertSelective(ExpDriverRead record);

    ExpDriverRead selectByPrimaryKey(@Param("driverId") Long driverId, @Param("dataNodeId") Integer dataNodeId);

    List<ExpDriverRead> selectByShardKey(Integer dataNodeId);

    int updateByPrimaryKeySelective(ExpDriverRead record);
}