package com.wanshun.fileexport.cardatamodel.generator.confservice;

import java.util.ArrayList;
import java.util.List;

import org.mybatis.generator.config.Configuration;
import org.mybatis.generator.config.xml.ConfigurationParser;
import org.mybatis.generator.internal.DefaultShellCallback;
import org.mybatis.generator.wanshun.WanShunMyBatisGenerator;

public class ConfServiceDataGenerate {
	
	public void generator(String xmlName) throws Exception{
		System.out.println("Generate mybatis file begin, xmlName=" + xmlName);

		List<String> warnings = new ArrayList<String>();
		boolean overwrite = false;
		
		//指定 逆向工程配置文件		
		ConfigurationParser cp = new ConfigurationParser(warnings);
		Configuration config = cp.parseConfiguration(ConfServiceDataGenerate.class.getResourceAsStream(xmlName));
		DefaultShellCallback callback = new DefaultShellCallback(overwrite);
		WanShunMyBatisGenerator myBatisGenerator = new WanShunMyBatisGenerator(config, callback, warnings);
		myBatisGenerator.generate(null);

		System.out.println("Generate mybatis file end, xmlName=" + xmlName);
	}
	
	public static void main(String[] args) {
		try {
			ConfServiceDataGenerate generatorSqlmap = new ConfServiceDataGenerate();
			generatorSqlmap.generator("confservice.xml");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
