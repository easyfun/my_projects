package com.wanshun.poolfile.springboot.config.datamodel;

import com.wanshun.common.database.interceptor.DataPermissionsInterceptor;
import com.wanshun.common.database.mongo.MongoConfig;
import com.wanshun.common.database.mongo.WsMorphiaDatastoreBean;
import com.wanshun.common.database.mysql.DataSourceConfigUtils;
import com.wanshun.common.database.mysql.DateSourceConfigBean;
import com.wanshun.common.utils.Base64;
import org.apache.ibatis.plugin.Interceptor;
import org.mybatis.spring.SqlSessionFactoryBean;
import org.mybatis.spring.annotation.MapperScan;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.env.Environment;
import org.springframework.core.io.Resource;
import org.springframework.core.io.support.PathMatchingResourcePatternResolver;
import org.springframework.core.io.support.ResourcePatternResolver;
import org.springframework.jdbc.datasource.DataSourceTransactionManager;
import org.springframework.transaction.interceptor.TransactionInterceptor;

import javax.sql.DataSource;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

@MapperScan({"com.wanshun.poolfile.cardatamodel.dao"})
@Configuration
public class DataModelConfig {

    public static Logger logger = LoggerFactory.getLogger(DataModelConfig.class);

    @Autowired
    private DateSourceConfigBean dateSourceConfigBean;

    @Autowired
    private MongoConfig mongoConfig;

    @Autowired
    private Environment environment;

	@Bean
	public WsMorphiaDatastoreBean carOrderMorphiaDatastoreBean() {
	    MongoConfig config = new MongoConfig();
        BeanUtils.copyProperties(mongoConfig, config);

        WsMorphiaDatastoreBean wsMorphiaDatastoreBean = new WsMorphiaDatastoreBean(config);
		return wsMorphiaDatastoreBean;
	}

//    @Bean
//    public RedisCache redisCache(@Autowired RedisConfig redisConfig) {
//        RedisCache bean = new RedisCache(DriverStatsModuleConstant.APPLICATION_NAME, redisConfig);
//        return bean;
//    }

    private DataSource driverDataSource() {
        logger.info("DriverSlaveDatabase config={}", dateSourceConfigBean.toString());
        String url = environment.getProperty("druid.driver.url");

        String value = environment.getProperty("druid.driver.username");
        String username = Base64.decode(value, "utf-8");

        value = environment.getProperty("druid.driver.password");
        String password = Base64.decode(value, "utf-8");

        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
    }

//    private DataSource confSlaveDataSource() {
//        logger.info("ConfSlaveDatabase config={}", dateSourceConfigBean.toString());
//        String url = environment.getProperty("druid.confServiceSlave.url");
//
//        String value = environment.getProperty("druid.confServiceSlave.username");
//        String username = Base64.decode(value, "utf-8");
//
//        value = environment.getProperty("druid.confServiceSlave.password");
//        String password = Base64.decode(value, "utf-8");
//
//        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
//    }

    private DataSource fileExportDataSource() {
        logger.info("carOrderStatsDatabase config={}", dateSourceConfigBean.toString());
        String url = environment.getProperty("druid.fileExport.url");

        String value = environment.getProperty("druid.fileExport.username");
        String username = Base64.decode(value, "utf-8");

        value = environment.getProperty("druid.fileExport.password");
        String password = Base64.decode(value, "utf-8");

        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
    }

    private DataSource passengerDataSource() {
        logger.info("PassengerSlaveDatabase config={}", dateSourceConfigBean.toString());
        String url = environment.getProperty("druid.passengerSlave.url");

        String value = environment.getProperty("druid.passengerSlave.username");
        String username = Base64.decode(value, "utf-8");

        value = environment.getProperty("druid.passengerSlave.password");
        String password = Base64.decode(value, "utf-8");

        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
    }

    @Bean(name = "dynamicDataSource")
    public DataSource dynamicDataSource() {
        DynamicDataSource dynamicDataSource = new DynamicDataSource();

        DataSource fileExport = fileExportDataSource();
        // 默认数据源
        dynamicDataSource.setDefaultTargetDataSource(fileExport);

        // 配置多数据源
        Map<Object, Object> dsMap = new HashMap(2);
        dsMap.put(DataSourceKey.fileExport, fileExport);

        DataSource driver = driverDataSource();
        dsMap.put(DataSourceKey.driver, driver);

//        DataSource confServiceSlave = confSlaveDataSource();
//        dsMap.put(DataSourceKey.confServiceSlave, confServiceSlave);

        DataSource passenger = passengerDataSource();
        dsMap.put(DataSourceKey.passenger, passenger);

        dynamicDataSource.setTargetDataSources(dsMap);
        return dynamicDataSource;
    }

    @Bean
    public SqlSessionFactoryBean sqlSessionFactory(@Autowired DataSource dynamicDataSource) {
        SqlSessionFactoryBean sqlSessionFactory = new SqlSessionFactoryBean();
        sqlSessionFactory.setDataSource(dynamicDataSource);

        try {
            //spring获取classpath下通配的文件列表解析器
            ResourcePatternResolver resourceLoader = new PathMatchingResourcePatternResolver();
            Resource[] resources = resourceLoader.getResources("com/wanshun/poolfile/cardatamodel/mapper/**/*.xml");
            sqlSessionFactory.setMapperLocations(resources);

            //根据需要增加
            Interceptor[] plugins = new Interceptor[1];
            plugins[0] = new DataPermissionsInterceptor();
            sqlSessionFactory.setPlugins(plugins);

            return sqlSessionFactory;

        } catch (IOException e) {
            logger.error("Load mybatis mapper xml exception, ", e);
        }

        return null;
    }

    @Bean
    public DataSourceTransactionManager transactionManager(@Autowired DataSource dynamicDataSource) {
        DataSourceTransactionManager bean = new DataSourceTransactionManager();
        bean.setDataSource(dynamicDataSource);
        return bean;
    }

    // 创建事务通知
    @Bean
    public TransactionInterceptor txAdvice(@Autowired DataSourceTransactionManager transactionManager)
            throws Exception {
        Properties properties = new Properties();
        properties.setProperty("*WithSqlTrans", "PROPAGATION_REQUIRED,-Throwable");

        TransactionInterceptor transactionInterceptor = new TransactionInterceptor(transactionManager, properties);
        return transactionInterceptor;
    }

}
