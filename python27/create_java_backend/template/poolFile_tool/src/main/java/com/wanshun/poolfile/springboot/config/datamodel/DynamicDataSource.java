package com.wanshun.poolfile.springboot.config.datamodel;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.jdbc.datasource.lookup.AbstractRoutingDataSource;

/**
 * 动态数据源
 **/
public class DynamicDataSource extends AbstractRoutingDataSource {
    private static final Logger logger = LoggerFactory.getLogger(DynamicDataSource.class);

    @Override
    protected Object determineCurrentLookupKey() {
//        logger.info("数据源为:{}", DataSourceContextHolder.getDB());

        return DataSourceContextHolder.getDB();
    }
}