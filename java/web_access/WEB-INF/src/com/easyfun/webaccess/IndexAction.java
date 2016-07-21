package com.easyfun.webaccess;

import java.util.HashMap;
import java.util.Map;

//import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

//import org.springframework.web.bind.ServletRequestUtils;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.Controller;

public class IndexAction implements Controller{
	private String viewPage;
	
	@Override
	public ModelAndView handleRequest(HttpServletRequest arg0, HttpServletResponse arg1) throws Exception {
		Map<String, String> model=new HashMap<String, String>();
		return new ModelAndView(getViewPage(), model);
	}

	public void setViewPage(String viewPage){
		this.viewPage=viewPage;
	}
	
	public String getViewPage(){
		return viewPage;
	}
}
