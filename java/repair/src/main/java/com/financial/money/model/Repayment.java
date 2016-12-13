package com.financial.money.model;

import java.util.Date;

public class Repayment {
	//还款id,借款人uid,标id,标题,还款期数,还款金额
	public long repayment_id;//   			bigint not null default 0 comment '还款计划ID',
	public long borrower_uid;
	public long asset_id;//        		bigint not null default 0 comment '标的id',   
	public int asset_type;//				tinyint not null default 0 comment '标的类型,0-散标,1-理财计划',
	public int asset_pool;//				tinyint not null default 0 comment '理财计划池/散标池,0-表示无,1-表示散标池,2-表示理财计划池,3-散标池和理财计划池',
	public String asset_name;//          	varchar(64) not null comment '产品标题',
	public int phase_total;//        		int not null default 0 comment '借款总期数',
	public int phase_mode;//				int not null default 0 comment '期数类型 0月 1天 2年',
	public int phase;//           		int not null default 0 comment '第几期还款',

	public long actual_principal;//bigint not null default 0 comment '本期实际已还本金',
	public long actual_interest;//  		bigint not null default 0 comment '本期实际已还利息',
	public long actual_penalty;//   		bigint not null default 0 comment '本期实际已缴罚金',	
	public long actual_compensation;//		bigint not null default 0 comment '本期实际已缴违约金',
	public long actual_pay_guarantee;//	bigint not null default 0 comment '本期实际担保商的费率',
	public long actual_pay_incoming;//		bigint not null default 0 comment '本期实际进件方的费率',
	public long actual_pay_platform;//		bigint not null default 0 comment '本期实际给平台的费率',
	public long actual_pay_fee;//			bigint not null default 0 comment '本期实际交纳的各种费用',
//	public Date actual_date;//				datetime not null default '0000-00-00 00:00:00' comment '实际清偿日期',

	public long getBorrower_uid() {
		return borrower_uid;
	}
	public void setBorrower_uid(long borrower_uid) {
		this.borrower_uid = borrower_uid;
	}
	public long getActual_principal() {
		return actual_principal;
	}
	public void setActual_principal(long actual_principal) {
		this.actual_principal = actual_principal;
	}
	public long getActual_interest() {
		return actual_interest;
	}
	public void setActual_interest(long actual_interest) {
		this.actual_interest = actual_interest;
	}
	public long getActual_penalty() {
		return actual_penalty;
	}
	public void setActual_penalty(long actual_penalty) {
		this.actual_penalty = actual_penalty;
	}
	public long getActual_compensation() {
		return actual_compensation;
	}
	public void setActual_compensation(long actual_compensation) {
		this.actual_compensation = actual_compensation;
	}
	public long getActual_pay_guarantee() {
		return actual_pay_guarantee;
	}
	public void setActual_pay_guarantee(long actual_pay_guarantee) {
		this.actual_pay_guarantee = actual_pay_guarantee;
	}
	public long getActual_pay_incoming() {
		return actual_pay_incoming;
	}
	public void setActual_pay_incoming(long actual_pay_incoming) {
		this.actual_pay_incoming = actual_pay_incoming;
	}
	public long getActual_pay_platform() {
		return actual_pay_platform;
	}
	public void setActual_pay_platform(long actual_pay_platform) {
		this.actual_pay_platform = actual_pay_platform;
	}
	public long getActual_pay_fee() {
		return actual_pay_fee;
	}
	public void setActual_pay_fee(long actual_pay_fee) {
		this.actual_pay_fee = actual_pay_fee;
	}
	public long getRepayment_id() {
		return repayment_id;
	}
	public void setRepayment_id(long repayment_id) {
		this.repayment_id = repayment_id;
	}
	public long getAsset_id() {
		return asset_id;
	}
	public void setAsset_id(long asset_id) {
		this.asset_id = asset_id;
	}
	public int getAsset_type() {
		return asset_type;
	}
	public void setAsset_type(int asset_type) {
		this.asset_type = asset_type;
	}
	public int getAsset_pool() {
		return asset_pool;
	}
	public void setAsset_pool(int asset_pool) {
		this.asset_pool = asset_pool;
	}
	public String getAsset_name() {
		return asset_name;
	}
	public void setAsset_name(String asset_name) {
		this.asset_name = asset_name;
	}
	public int getPhase_total() {
		return phase_total;
	}
	public void setPhase_total(int phase_total) {
		this.phase_total = phase_total;
	}
	public int getPhase_mode() {
		return phase_mode;
	}
	public void setPhase_mode(int phase_mode) {
		this.phase_mode = phase_mode;
	}
	public int getPhase() {
		return phase;
	}
	public void setPhase(int phase) {
		this.phase = phase;
	}
	@Override
	public String toString() {
		return "Repayment [repayment_id=" + repayment_id + ", borrower_uid=" + borrower_uid + ", asset_id=" + asset_id
				+ ", asset_type=" + asset_type + ", asset_pool=" + asset_pool + ", asset_name=" + asset_name
				+ ", phase_total=" + phase_total + ", phase_mode=" + phase_mode + ", phase=" + phase
				+ ", actual_principal=" + actual_principal + ", actual_interest=" + actual_interest
				+ ", actual_penalty=" + actual_penalty + ", actual_compensation=" + actual_compensation
				+ ", actual_pay_guarantee=" + actual_pay_guarantee + ", actual_pay_incoming=" + actual_pay_incoming
				+ ", actual_pay_platform=" + actual_pay_platform + ", actual_pay_fee=" + actual_pay_fee + "]";
	}
	
	public String title() {
		return "还款id,借款人uid,标id,标题,还款期数,费用,还款本息\n";
	}
	
	public String getOut() {
		return String.format("%d,%d,%d,%s,%d,%d,%d\n", 
				repayment_id,
				borrower_uid,
				asset_id,
				asset_name,
				phase,
				actual_penalty+actual_compensation+actual_pay_guarantee+actual_pay_incoming+actual_pay_platform+actual_pay_fee,
				actual_principal+actual_interest);
	}
}
