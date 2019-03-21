package com.wanshun.poolfile.cardatamodel.generator.order;

import java.util.ArrayList;
import java.util.List;

import org.mybatis.generator.config.Configuration;
import org.mybatis.generator.config.xml.ConfigurationParser;
import org.mybatis.generator.internal.DefaultShellCallback;
import org.mybatis.generator.wanshun.WanShunMyBatisGenerator;

public class OrderGenerate {

	public void common(String xmlName) throws Exception{
		List<String> warnings = new ArrayList<String>();
		boolean overwrite = false;
		
		//指定 逆向工程配置文件		
		ConfigurationParser cp = new ConfigurationParser(warnings);
		Configuration config = cp.parseConfiguration(OrderGenerate.class.getResourceAsStream(xmlName));
		DefaultShellCallback callback = new DefaultShellCallback(overwrite);
		WanShunMyBatisGenerator myBatisGenerator = new WanShunMyBatisGenerator(config, callback, warnings);
		myBatisGenerator.generate(null);
		System.out.println(xmlName);
	}
	
	public void generator() throws Exception{
		String xmlName="order.xml";
		common(xmlName);
	}
	
	public static void main(String[] args) {
		try {
			OrderGenerate generatorSqlmap = new OrderGenerate();
			generatorSqlmap.generator();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}
}
