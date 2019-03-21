package com.wanshun.fileexport.cardatamodel.mongo.dao.carorder;

import com.mongodb.BasicDBObject;
import com.wanshun.common.database.mongo.WsMongoBaseDao;
import com.wanshun.common.database.mongo.WsMorphiaDatastoreBean;
import com.wanshun.common.utils.PageUtils;
import com.wanshun.common.utils.PageUtils.WsRepeatCompare;
import com.wanshun.common.utils.StringUtil;
import com.wanshun.fileexport.cardatamodel.mongo.po.carorder.OrderExpCarInstantMPO;
import org.apache.commons.collections.IteratorUtils;
import org.mongodb.morphia.Datastore;
import org.mongodb.morphia.aggregation.Accumulator;
import org.mongodb.morphia.aggregation.AggregationPipelineImpl;
import org.mongodb.morphia.aggregation.Group;
import org.mongodb.morphia.query.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import java.util.*;

@Repository
public class OrderExpCarInstantMDao extends WsMongoBaseDao<OrderExpCarInstantMPO> {

	public OrderExpCarInstantMDao(@Autowired WsMorphiaDatastoreBean carOrderMorphiaDatastoreBean) {
		super(carOrderMorphiaDatastoreBean.getDatastore());
	}

	@Override
	protected Query<OrderExpCarInstantMPO> getShardKey(OrderExpCarInstantMPO entity) {
		Query<OrderExpCarInstantMPO> query = datastore.createQuery(getEntityClass());

		query.filter("orderId =", entity.getOrderId());
		
		if(null != entity.getCreateTime()){
			query.filter("createTime", entity.getCreateTime());
		}

		return query;
	}

	@Override
	protected Query<OrderExpCarInstantMPO> getPrimaryKey(OrderExpCarInstantMPO entity) {
		Query<OrderExpCarInstantMPO> query = getShardKey(entity);
		
		return query;
	}
//
//	/**
//	 * 根据订单号返回数据
//	 */
//	public OrderExpCarInstantMPO getExpressOrderByOrderId(ExpOrderCondition expOrderCondition){
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		setQuery(expOrderCondition, query);
//		OrderExpCarInstantMPO orderExpCarInstantMPO = query.get();
//		return orderExpCarInstantMPO;
//	}
//
//	/**
//	 * 网约车订单列表(基于时间分页)
//	 */
//	public List<OrderExpCarInstantMPO> getExpressOrderList(ExpOrderCondition expOrderCondition){
//		List<OrderExpCarInstantMPO> list = new ArrayList<>();
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		setQuery(expOrderCondition, query);//设置参数
//		if(null == expOrderCondition.getNext()) {
//			query.order(Sort.descending("createTime"));//根据订单创建时间降序
//		}else if(expOrderCondition.getNext() == -1) {
//			query.order(Sort.ascending("createTime"));//根据订单创建时间升序
//		}else{
//			query.order(Sort.descending("createTime"));//根据订单创建时间降序
//		}
//		FindOptions findOptions = new FindOptions();
//		findOptions.limit(expOrderCondition.getPageSize());
//		list.addAll(query.asList(findOptions));
//		return list;
//	}
//
//	/**
//	 *  网约车订单列表总条数
//	 */
//	public Long getExpressOrderCount(ExpOrderCondition expOrderAo){
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		setQuery(expOrderAo, query);//设置参数
//		return query.count();
//	}
//
//	/**
//	 * 根据条件查找所有订单
//	 */
//	public List<OrderExpCarInstantMPO> findList(ExpOrderCondition expOrderCondition){
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		setQuery(expOrderCondition, query);//设置参数
//		return query.asList();
//	}
//
//	/**
//	 * 条件封装
//	 */
//	private void setQuery(ExpOrderCondition condition, Query<OrderExpCarInstantMPO> query) {
//		if(!StringUtil.isNullOrBlank(condition.getOrderId())){//订单号
//			query.field("orderId").equal(condition.getOrderId());
//		}
//		if(!StringUtil.isNullOrBlank(condition.getDriverId())){//司机ID
//			query.field("driverId").equal(condition.getDriverId());
//		}
//		if (null != condition.getDriverIds() && condition.getDriverIds().size() > 0) {
//			query.field("driverId").equal(new BasicDBObject("$in", condition.getDriverIds()));
//		}
//		if(!StringUtil.isNullOrBlank(condition.getAppUserId())){//用户ID
//			query.field("appUserId").equal(condition.getAppUserId());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getCreateStartTime())) {//订单添加时间  起
//			query.field("createTime").greaterThanOrEq(condition.getCreateStartTime());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getCreateEndTime())) {//订单添加时间  止
//			query.field("createTime").lessThanOrEq(condition.getCreateEndTime());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getPayStartTime())) {//订单支付时间  起
//			query.field("payTime").greaterThanOrEq(condition.getPayStartTime());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getPayEndTime())) {//订单支付时间  止
//			query.field("payTime").lessThanOrEq(condition.getPayEndTime());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getPayType())) {//订单支付类型
//			query.field("payType").equal(condition.getPayType());
//		}
//		if (!StringUtil.isNullOrBlank(condition.getStatus())) {//订单支付状态
//			if (condition.getStatus().equals(-50)) {//已关闭
//				query.field("status").in(Arrays.asList(OrderAttributeConst.EXP_INSTANT_DEAD,OrderAttributeConst.EXP_INSTANT_SYSTEMADMIN_CANCEL,OrderAttributeConst.EXP_INSTANT_APPUSER_CANCEL,
//						OrderAttributeConst.EXP_INSTANT_DRIVER_CANCEL,OrderAttributeConst.EXP_INSTANT_NOT_DISPATCH));
//			}else{
//				query.field("status").equal(condition.getStatus());
//			}
//		}
//		if (!StringUtil.isNullOrBlank(condition.getPassengerPhone())) {//乘客电话
//			query.field("passengerPhone").equal(condition.getPassengerPhone());
//		}
//
//		if(condition.getNext() == null) {
//
//		}else if(condition.getNext() == 1){
//			if(condition.getLastTime() != null) {
//				query.field("createTime").lessThan( condition.getLastTime());
//			}
//
//		}else if(condition.getNext() == -1){
//			if(condition.getFirstTime() != null) {
//				query.field("createTime").greaterThan( condition.getFirstTime());
//			}
//		}
//		if (!StringUtil.isNullOrBlank(condition.getOrderType())) {//订单类型
//			query.field("level").equal(condition.getOrderType());
//		}
//
//		if (!StringUtil.isNullOrBlank(condition.getAgencyNumber())) {//区分权限
//			query.field("agencyNumber").contains(condition.getAgencyNumber());
//		}
//
//	}
//
//	public List<OrderExpCarInstantMPO> getPaidOrderList(ExpOrderCondition expOrderCondition){
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		if (!StringUtil.isNullOrBlank(expOrderCondition.getPayStartTime())) {//订单支付时间  起
//			query.field("payTime").greaterThanOrEq(expOrderCondition.getPayStartTime());
//		}
//		if (!StringUtil.isNullOrBlank(expOrderCondition.getPayEndTime())) {//订单支付时间  止
//			query.field("payTime").lessThan(expOrderCondition.getPayEndTime());
//		}
//		if(!StringUtil.isNullOrBlank(expOrderCondition.getStatus())){// 根据订单支付状态
//			query.field("status").equal(expOrderCondition.getStatus());
//		}
//
//		if (!StringUtil.isNullOrBlank(expOrderCondition.getLastOrderId())) {//订单支付时间  止
//			query.field("orderId").greaterThan(expOrderCondition.getLastOrderId());
//		}
//		query.order(Sort.ascending("orderId"));//根据订单ID升序
//
//		FindOptions findOptions = new FindOptions();
//		findOptions.limit(expOrderCondition.getPageSize());
//		List<OrderExpCarInstantMPO> list = new ArrayList<>();
//		list.addAll(query.asList(findOptions));
//
//		return list;
//	}
//
//	public List<OrderExpCarInstantMPO> getPageListByPayTime(Date startDate, Date endDate, Integer statement, Integer status, Integer pageSize, String lastOrderId){
//		List<OrderExpCarInstantMPO> list = new ArrayList<>();
//
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		query.order(Sort.ascending("payTime"));//根据支付时间降序
//
//		query.field("payTime").lessThanOrEq(endDate);
//		query.field("payTime").greaterThanOrEq(startDate);
//		query.field("status").equal(status);
//		query.field("statement").equal(statement);
//
//		FindOptions findOptions = new FindOptions();
//		findOptions.limit(pageSize);
//		list.addAll(query.asList(findOptions));
//
//		PageUtils.removeRepeat(list, lastOrderId, new WsRepeatCompare<OrderExpCarInstantMPO, String>(){
//
//			@Override
//			public boolean compare(OrderExpCarInstantMPO entity, String lastId) {
//				if( entity.getOrderId().equals(lastId) ){
//					return true;
//				}
//				return false;
//			}
//
//		});
//
//		return list;
//	}
//
//
//	public List<OrderExpCarInstantMPO> getExpAllOrderForRecove(Long driverId, Date startDate) {
//
//		List<OrderExpCarInstantMPO> list = new ArrayList<>();
//
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		query.order(Sort.descending("payTime"));//根据支付时间降序
//
//		query.field("payTime").greaterThan(startDate);
//		query.field("driverId").equal(driverId);
//		//query.field("payType").notEqual(OrderAttributeConst.ORDER_PAY_BALANCE);
//		query.field("status").equal(OrderAttributeConst.EXP_INSTANT_COMPLETE);
//
//		list.addAll(query.asList());
//
//		return list;
//
//	}
//
//	/**
//	 * 已付款订单每小时统计
//	 */
//	public List<ExpDayHour> getExpOrderInstantSum(Map<String,Object> mapParams) {
//		List<ExpDayHour> list = new ArrayList<ExpDayHour>();
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//		if(mapParams.get("agencyNumber")!=null){
//			query.field("agencyNumber").equal(mapParams.get("agencyNumber"));
//		}
//		if(mapParams.get("level")!=null){
//			query.field("level").equal(mapParams.get("level"));
//		}
//		if(mapParams.get("status")!=null){
//			query.field("status").equal(mapParams.get("status"));
//		}
//		if(mapParams.get("payTimeStart")!=null){
//			query.field("payTime").greaterThanOrEq(mapParams.get("payTimeStart"));
//		}
//		if(mapParams.get("payTimeEnd")!=null){
//			query.field("payTime").lessThan(mapParams.get("payTimeEnd"));
//		}
//		if(mapParams.get("confirmPayTimeStart")!=null){
//			query.field("confirmPayTime").greaterThanOrEq(mapParams.get("confirmPayTimeStart"));
//		}
//		if(mapParams.get("confirmPayTimeEnd")!=null){
//			query.field("confirmPayTime").lessThan(mapParams.get("confirmPayTimeEnd"));
//		}
//		AggregationPipelineImpl pipeline = (AggregationPipelineImpl) datastore.createAggregation(OrderExpCarInstantMPO.class).match(query);
//		pipeline.group(Group.grouping("_id","agencyNumber")
//				,Group.grouping("agencyNumber",Group.first("agencyNumber"))
//				,Group.grouping("orgType",Group.first("orgType"))
//				,Group.grouping("orderPrice",Group.sum("orderPrice"))
//				,Group.grouping("driverIncome", Group.sum("driverIncome"))
//				,Group.grouping("platformChargeMoney", Group.sum("platformChargeMoney"))
//				,Group.grouping("realPayPrice", Group.sum("realPayPrice"))
//				,Group.grouping("paidMoney", Group.sum("paidMoney"))
//				,Group.grouping("discountPrice", Group.sum("discountPrice"))
//				,Group.grouping("orderNum", new Accumulator("$sum", 1)));
//		Iterator<ExpDayHour> result = pipeline.aggregate(ExpDayHour.class);
//		if(null == result || !result.hasNext()){
//			return null;
//		}
//		list = IteratorUtils.toList(result);
//		return list;
//	}
//
//	public Integer updateStatement(OrderExpCarInstantMPO order){
//		Query<OrderExpCarInstantMPO> query = datastore.createQuery(OrderExpCarInstantMPO.class);
//
//		query.field("orderId").equal(order.getOrderId());
//
//		UpdateOperations<OrderExpCarInstantMPO> uo = datastore.createUpdateOperations(OrderExpCarInstantMPO.class).set("statement", order.getStatement());
//		if(null != order.getPayTime()){
//			uo.set("payTime", order.getPayTime());
//		}
//
//		if(null != order.getCheckStatus()){
//			uo.set("checkStatus", order.getCheckStatus());
//		}
//
//		if( null != order.getBalanceMoney() ){
//			uo.set("balanceMoney", order.getBalanceMoney());
//		}
//
//		if( null != order.getDiscountPrice() ){
//			uo.set("discountPrice", order.getDiscountPrice());
//		}
//
//		if(null != order.getRealPayPrice()){
//			uo.set("realPayPrice", order.getRealPayPrice());
//		}
//
//		if(null != order.getPaidMoney()){
//			uo.set("paidMoney", order.getPaidMoney());
//		}
//
//		if(null != order.getOrderPrice()){
//			uo.set("orderPrice", order.getOrderPrice());
//		}
//
//		UpdateResults result = datastore.update(query, uo);
//
//		return result.getUpdatedCount();
//	}
}