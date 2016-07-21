package com.easyfun.webaccess;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Scanner;

//import java.util.HashMap;
//import java.util.Map;

//import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.map.SerializationConfig;
//import org.springframework.web.bind.ServletRequestUtils;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.Controller;

public class MonitorPlatformSigninAction implements Controller{
	@Override
	public ModelAndView handleRequest(HttpServletRequest arg0, HttpServletResponse arg1) throws Exception {
		//printRequest(arg0);
		ObjectMapper mapper=new ObjectMapper();
		mapper.configure(SerializationConfig.Feature.INDENT_OUTPUT, Boolean.TRUE);
/*		String inJson=extractPostRequestBody(arg0);
		System.out.println("\n\ninJson");
		System.out.println(inJson);
		for (int i=0;i<inJson.length();i++){ 
			int ch = (int)inJson.charAt(i); 
			System.out.print(Integer.toHexString(ch)); 
		}
*/
		AdminSigninForm form=mapper.readValue(arg0.getInputStream(), AdminSigninForm.class);
		System.out.println(form.getName());
		System.out.println(form.getPassword());

		String inJson=extractPostRequestBody(arg0);
		System.out.println("\n\ninJson");
		System.out.println(inJson);

		
/*		AdminSigninForm outForm = new AdminSigninForm();
		outForm.setName("admin");
		outForm.setPassword("ok");
		String outJson=mapper.writeValueAsString(outForm);
		System.out.println("\n\noutJson from form");
		System.out.println(outJson);
		for (int i=0;i<outJson.length();i++){ 
			int ch = (int)outJson.charAt(i); 
			System.out.print(Integer.toHexString(ch)); 
		}
		System.out.println("");
		outJson="{\"name\":\"admin\",\"password\":\"ok\"}";
		System.out.println("\n\noutJson string");
		System.out.println(outJson);
		for (int i=0;i<outJson.length();i++){ 
			int ch = (int)outJson.charAt(i); 
			System.out.print(Integer.toHexString(ch)); 
		}
		System.out.println("");*/
		arg1.setContentType("appliction/json");
		arg1.setCharacterEncoding("utf-8");
		arg1.getWriter().println("{\"code\":0,\"info\":\"ok\"}");
		return null;
	}
	
	@SuppressWarnings("unused")
	private void printRequest(HttpServletRequest httpRequest) {
	    System.out.println(" \n\n Headers");

	    Enumeration<String> headerNames = httpRequest.getHeaderNames();
	    while(headerNames.hasMoreElements()) {
	        String headerName = (String)headerNames.nextElement();
	        System.out.println(headerName + " = " + httpRequest.getHeader(headerName));
	    }

	    System.out.println("\n\nParameters");

	    Enumeration<String> params = httpRequest.getParameterNames();
	    while(params.hasMoreElements()){
	        String paramName = (String)params.nextElement();
	        System.out.println(paramName + " = " + httpRequest.getParameter(paramName));
	    }

	    System.out.println("\n\n Row data");
	    System.out.println(extractPostRequestBody(httpRequest));
	}

	@SuppressWarnings("resource")
	static String extractPostRequestBody(HttpServletRequest request) {
	    if ("POST".equalsIgnoreCase(request.getMethod())) {
	        Scanner s = null;
	        try {
	            s = new Scanner(request.getInputStream(), "UTF-8").useDelimiter("\\A");
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	        return s.hasNext() ? s.next() : "";
	    }
	    return "";
	}
}

class AdminSigninForm {
	private String name;
	private String password;
	
	public AdminSigninForm(){
		name="";
		password="";
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name=name;
	}
	
	public String getPassword(){
		return password;
	}
	
	public void setPassword(String password){
		this.password=password;
	}
}