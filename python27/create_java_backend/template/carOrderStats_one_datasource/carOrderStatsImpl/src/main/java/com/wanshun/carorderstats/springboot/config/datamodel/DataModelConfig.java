package {{project_package_name}}.springboot.config.datamodel;

import com.wanshun.common.database.interceptor.DataPermissionsInterceptor;
import com.wanshun.common.database.mysql.DateSourceConfigBean;
import com.wanshun.common.utils.Base64;
import com.wanshun.common.database.mysql.DataSourceConfigUtils;
import org.apache.ibatis.plugin.Interceptor;
import org.mybatis.spring.SqlSessionFactoryBean;
import org.mybatis.spring.annotation.MapperScan;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
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
import java.util.*;

@MapperScan({"{{project_package_name}}.cardatamodel.dao"})
@Configuration
public class DataModelConfig {

    public static Logger logger = LoggerFactory.getLogger(DataModelConfig.class);

    @Autowired
    private DateSourceConfigBean dateSourceConfigBean;

    @Autowired
    private Environment environment;

//	@Bean
//	public MorphiaDatastoreBean morphiaDatastoreBean() {
//		MorphiaDatastoreBean morphiaDatastoreBean = new MorphiaDatastoreBean();
//		return morphiaDatastoreBean;
//	}

//    @Bean
//    public RedisCache redisCache(@Autowired RedisConfig redisConfig) {
//        RedisCache bean = new RedisCache(DriverStatsModuleConstant.APPLICATION_NAME, redisConfig);
//        return bean;
//    }

    @Bean
    public DataSource dataSource() {
        logger.info("DataSource config={}", dateSourceConfigBean.toString());
        String url = dateSourceConfigBean.getUrl();

        String value = dateSourceConfigBean.getUserName();
        String username = Base64.decode(value, "utf-8");

        value = dateSourceConfigBean.getPassword();
        String password = Base64.decode(value, "utf-8");

        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
    }

    @Bean
    public SqlSessionFactoryBean sqlSessionFactory(@Autowired DataSource dataSource) {
        SqlSessionFactoryBean sqlSessionFactory = new SqlSessionFactoryBean();
        sqlSessionFactory.setDataSource(dataSource);

        try {
            //spring获取classpath下通配的文件列表解析器
            ResourcePatternResolver resourceLoader = new PathMatchingResourcePatternResolver();
            Resource[] resources = resourceLoader.getResources("{{project_mybatis_resources}}/cardatamodel/mapper/**/*.xml");
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
    public DataSourceTransactionManager transactionManager(@Autowired DataSource dataSource) {
        DataSourceTransactionManager bean = new DataSourceTransactionManager();
        bean.setDataSource(dataSource);
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
