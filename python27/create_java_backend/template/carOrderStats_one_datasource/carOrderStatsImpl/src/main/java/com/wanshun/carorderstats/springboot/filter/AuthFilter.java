package {{project_package_name}}.springboot.filter;

import com.wanshun.common.code.anwser.AnwserCode;
import com.wanshun.common.msg.AppPacket;
import com.wanshun.common.utils.JsonUtil;
import com.wanshun.common.utils.NetUtils;
import com.wanshun.common.utils.PrintWriterUtil;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

/**
 * 权限校验filter
 *
 */
public class AuthFilter implements Filter {

	@Override
	public void destroy() {
	}

	@Override
	public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain)
			throws IOException, ServletException {
		HttpServletRequest request = (HttpServletRequest) servletRequest;
		HttpServletResponse response = (HttpServletResponse) servletResponse;
		
		String ipAddress = NetUtils.getIpAddress(request);

		boolean checkValid = NetUtils.checkIfInnerIp(ipAddress);
		
		// 来自内网的ip给予放行
		if (checkValid) {
			filterChain.doFilter(servletRequest, servletResponse);
		} else {
			AppPacket rtvPacket = new AppPacket();
			rtvPacket.setCode(AnwserCode.REQUEST_ILLEGAL.getCode());
			rtvPacket.setMsg(AnwserCode.REQUEST_ILLEGAL.getMessage());
			PrintWriterUtil.writeResultToClient(response, JsonUtil.toJson(rtvPacket));
		}
	}

	@Override
	public void init(FilterConfig arg0) {
		
	}
}
