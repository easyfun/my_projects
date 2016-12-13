package com.zyxr.repair.dao;

import java.io.InputStream;

import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

public class DbManager {

	public static SqlSessionFactory getSqlFactory() {
		String resource = "db_conf.xml";
		InputStream is = DbManager.class.getClassLoader().getResourceAsStream(resource);
		SqlSessionFactory sessionFactory = new SqlSessionFactoryBuilder().build(is);
		return sessionFactory;
	}
	
}
