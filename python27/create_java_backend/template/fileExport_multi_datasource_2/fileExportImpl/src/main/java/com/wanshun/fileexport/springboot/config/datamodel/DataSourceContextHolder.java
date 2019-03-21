package com.wanshun.fileexport.springboot.config.datamodel;

import com.wanshun.fileexport.cardatamodel.DataSourceKey;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * 当前线程数据源
 * */
public class DataSourceContextHolder {

    public static final Logger log = LoggerFactory.getLogger(DataSourceContextHolder.class);

    private static final ThreadLocal<DataSourceKey> contextHolder = new ThreadLocal<>();

    // 设置数据源名
    public static void setDB(DataSourceKey dbType) {
        log.info("切换到{}数据源", dbType);
        contextHolder.set(dbType);
    }

    // 获取数据源名
    public static DataSourceKey getDB() {
        return contextHolder.get();
    }

    // 清除数据源名
    public static void clearDB() {
        contextHolder.remove();
    }
}
