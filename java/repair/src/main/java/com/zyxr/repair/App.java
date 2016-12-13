package com.zyxr.repair;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.List;

import org.apache.log4j.PropertyConfigurator;

import com.financial.money.FinancialMoney;
import com.zyxr.feecalculator.FeeConstant;
import com.zyxr.feecalculator.Repayment;
import com.zyxr.feecalculator.RepaymentSchedule;
import com.zyxr.repair.utils.Tool;

public class App 
{
    public static void main( String[] args )
    {
//    	PropertyConfigurator.configure("log4j.properties");
//        RepaymentMissionLoan ra=new RepaymentMissionLoan();
//        ra.doJob();

//        RepaymentLoan rl=new RepaymentLoan();
//        rl.doJob();
    	
    	FinancialMoney financialMoney=new FinancialMoney();
    	financialMoney.doJob();
    	
//    	RepaymentedLoan2 r2=new RepaymentedLoan2();
//    	r2.doJob();
    	
//    	RepaymentMissionLoan2 repaymentMissionLoan2=new RepaymentMissionLoan2();
//    	repaymentMissionLoan2.doJob();
    }
    
    public static void repairRepaymets() {
		File file=new File("D:\\job_projects\\python\\mission_loan.txt");
		InputStream is=null;
		StringBuffer fileContent=new StringBuffer();
		int n=0;
		try {
			is = new FileInputStream(file);
			BufferedReader reader=new BufferedReader(new InputStreamReader(is));
			while (true) {
				n++;
				String line=reader.readLine();
				if (null==line) {
					break;
				}
				System.err.println("line="+line);
				if (n==1) {
					continue;
				}
//				if (n>1) {
//					System.out.println("只测一条");
//					break;
//				}
				String r=line.replace("\r", "");
				String l=r.replace("\n","");
				//System.out.println(l);
				String[] values=l.split(",");
				//System.out.println(gs.toJson(values));
				
				String loanId=values[0].trim();
				long capital=Tool.getMoney(values[1].trim());
				long annualRate=Tool.getMoney(values[2].trim());
				long periods=Long.valueOf(values[3].trim());
				long periodType=FeeConstant.LOAN_MODE_DAY;
				String valueDate="2016-09-21 00:00:00";		
				RepaymentSchedule rs=new RepaymentSchedule(capital,annualRate,periods,periodType,valueDate);
				rs.calRepaymentSchedule(FeeConstant.REPAYMENT_MODE_XXHB);

				//loan_id,periodNo,capital,interest
				String v=null;
				List<Repayment> repayments=rs.getRepaymentList();
				for (Repayment repayment:repayments) {
					v=String.format("%s,%d,%d,%d\n", loanId,repayment.getPeriodNo(),repayment.getCapital(),repayment.getInterest());
					fileContent.append(v);
				}
			}
			
			if (fileContent.length() > 0) {
				File w=new File("D:\\job_projects\\python\\cal_repay_schedule.txt");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileContent.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
    }
    
}
