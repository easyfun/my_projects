package com.wanshun.fileexport.service;


import com.wanshun.fileexport.cardatamodel.po.confservice.LocalCityRead;
import com.wanshun.fileexport.service.switchds.confservice.LocalCityService;
import com.wanshun.fileexport.service.switchds.driver.ExpDriverService;
import com.wanshun.fileexport.springboot.FileExportApplication;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

@RunWith(SpringJUnit4ClassRunner.class)
@SpringBootTest(classes = FileExportApplication.class)
public class DynamicDataSourceTest {
    private static final Logger logger = LoggerFactory.getLogger(DynamicDataSourceTest.class);

    @Autowired
    private LocalCityService localCityService;

    @Autowired
    private ExpDriverService expDriverService;

    @Test
    public void test() {
        String cityName = localCityService.getName(110100);
        logger.info("cityName={}", cityName);

        String phone = expDriverService.getPhone(10120010L);
        logger.info("phone={}", phone);
    }
}
