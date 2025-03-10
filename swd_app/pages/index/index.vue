<!-- app界面设置 -->
<template>
	<!-- 整体布局 -->
	<view class="wrap">
		<!-- 设备区域 -->
		<view class="dev-area">
			<!-- -卡片-- 实时温度 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">实时温度</view>
					<image class="dev-logo" src="/static/water_temp.png" mode=""></image>
				</view>
				<view class="dev-data">{{temp}} ℃</view>
			</view>
			<!-- -卡片-- 水泵（常规模式）有水 -->
			<view v-if="mode==false && water_level==false" class="dev-card">
				<view class="">
					<view class="dev-name">水泵</view>
					<image class="dev-logo" src="/static/water_switch.png" mode=""></image>
				</view>
				<switch :checked="pump" @change="sendPumpCom" color="#1dbe44" />
			</view>
			<!-- 卡片--水泵（加热模式，温度低于温度下限） -->
			<view v-else-if="Number(temp) < Number(set_temp_l) && water_level==false" class="dev-card" style="background-color: red;">
				加热模式-正在加热
			</view>
			<!-- 卡片--水泵 	缺水 -->
			<view v-else-if="water_level==true" class="dev-card" style="background-color: red;">
				缺水
			</view>
			<!-- 卡片--水泵（加热模式，温度高于温度下限 -->
			<view v-else class="dev-card">
				<view class="">
					<view class="dev-name">水泵</view>
					<image class="dev-logo" src="/static/water_switch.png" mode=""></image>
				</view>
				<switch :checked="pump" @change="sendPumpCom" color="#1dbe44" />
			</view>
			
			<!-- -卡片-- 模式 有水-->
			<view v-if="water_level==false" class="dev-card">
				<view class="">
					<view class="dev-name">加热模式</view>
					<image class="dev-logo" src="/static/hot_water.png" mode=""></image>
				</view>
				<switch :checked="mode" @change="sendModeCom" color="#1dbe44" />
			</view>
			<!-- 卡片--模式--缺水 -->
			<view v-else class="dev-card" style="background-color: red;">
				缺水
			</view>
			<!-- -卡片-- 水位报警 - 报警 -->
			<view v-if="water_level==true" class="dev-card">
				<view class="">
					<view class="dev-name">水位</view>
					<image class="dev-logo" src="/static/level_alarm.png" mode=""></image>
				</view>
			</view>
			<!-- 卡片--水位报警 - 不报警 -->
			<view v-else class="dev-card">
				<view class="">
					<view class="dev-name">水位</view>
					<image class="dev-logo" src="/static/level_alarm2.png" mode=""></image>
				</view>
			</view>
			<!-- 阈值长卡片--温度下限 -->
			<view class="dev-card-1">
				<view class="">
					<view class="dev-name">温度低阈值</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="set_temp_l" @change="sliderChangeL($event, 'slider1')" mim='0' max='80' step="5"
						block-size="20" show-value />
				</view>
				
			</view>
			<!-- 阈值长卡片--温度上限 -->
			<view class="dev-card-1">
				<view class="">
					<view class="dev-name">温度高阈值</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="set_temp_h" @change="sliderChangeH($event, 'slider1')" mim='0' max='80' step="5"
						block-size="20" show-value />
				</view>
				
			</view>

		</view>
	</view>

</template>

<script>
	// 引入字符串处理函数
	import{
		stringify
	} from 'querystring';
	// 引入通用令牌函数(计算token)
	const {
		createCommonToken
	} = require('@/key.js')
	// 产品ID,设备名,密钥,用户名需要替换成自己的
	
	const product_id = 'mHB0WKVSN0';
	const device_name = 'd1';
	const user_accessKey = 'gxVhtL68CPQo1kvLOyOpIfiyfZp/eaV8uciafPtfxvFjiKatrsWquQKW0LKMj1DQ';
	const userID = '412251';
	
	export default {
		// 数据部分
		data() {
			return {
				// 实时温度
				temp: '',
				// 温度上限
				set_temp_h: '',
				// 温度下限
				set_temp_l: '',
				// 水泵
				pump: false,
				// 模式
				mode: false,
				// 水位报警
				water_level: false,
				// 接口请求token
				token: '',
				
				//低温度阈值标志位
				temp_l_flag: 0,
				//高温度阈值标志位
				temp_h_flag: 0,
				//水泵开关标志位
				pump_flag: 0,
				//模式切换标志位
				mode_flag: 0,
			}
		},
		onLoad() {
			const params = {
				author_key: user_accessKey,
				version: '2022-05-01',
				user_id: userID,
			}
			this.token = createCommonToken(params);
		},
		onShow() {
			// 首次获取设备数据
			this.fetchDevData();
			// 定时3秒获取数据
			setInterval(() => {
				this.fetchDevData();
			}, 3000)
		},
		// 方法部分
		methods: {
			// 水泵开关方法
			sendPumpCom(pump_event) {
				console.log(pump_event.detail.value);
				this.pump_flag = 2;
				let pump_value = pump_event.detail.value;

				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: product_id,
						device_name: device_name,
						params: {
							"pump": pump_value,
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('PUMP' + (pump_value ? ' ON' : ' OFF') + '!');
					}
				});
			},
			// 模式切换方法
			sendModeCom(mode_event) {
				console.log(mode_event.detail.value);
				this.mode_flag = 2;
				let mode_value = mode_event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: product_id,
						device_name: device_name,
						params: {
							"mode": mode_value,
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('mode' + (mode_value ? ' ON' : ' OFF') + '!');
					}
				});
			},
			// 温度下限滑动条变化事件方法
			sliderChangeL(e_l, id){
				console.log('set_temp_l 发生变化：' + e_l.detail.value)
				this.temp_l_flag = 2;
				let temp_l_th = {}
				this.set_temp_l = e_l.detail.value
				this.temp_l_th = {set_temp_l: this.set_temp_l,}
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: product_id,
						device_name: device_name,
						params: this.temp_l_th
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('温度下限 ' + this.temp_l_th);
					}
				});
			},
			// 温度上限滑动条变化事件方法
			sliderChangeH(e_h, id){
				console.log('set_temp_h 发生变化：' + e_h.detail.value)
				this.temp_h_flag = 2;
				let temp_h_th = {}
				this.set_temp_h = e_h.detail.value
				this.temp_h_th = {set_temp_h: this.set_temp_h,}
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: product_id,
						device_name: device_name,
						params: this.temp_h_th
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('温度上限 ' + this.temp_h_th);
					}
				});
			},
			// 获取设备数据方法
			fetchDevData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //仅为示例，并非真实接口地址。
					method: 'GET',
					data: {
						product_id: product_id,
						device_name: device_name
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: (res) => {
						console.log(res.data);
						if(this.mode_flag==0)this.mode = res.data.data[0].value === 'true' ? true : false;
						else this.mode_flag = this.mode_flag - 1;
						
						if(this.pump_flag==0)this.pump = res.data.data[1].value === 'true' ? true : false;
						else this.pump_flag=this.pump_flag - 1;
						
						if(this.temp_h_flag==0)this.set_temp_h = res.data.data[2].value;
						else this.temp_h_flag=this.temp_h_flag - 1;
						
						if(this.temp_l_flag==0)this.set_temp_l = res.data.data[3].value;
						else this.temp_l_flag=this.temp_l_flag - 1;
						
						this.temp = res.data.data[4].value;
						this.water_level = res.data.data[5].value === 'true' ? true : false;
					}
				});
			}

		}
	}
</script>

<style>
	/* 整体页面样式 */
	.wrap {
		/* 设备内边距30像素 */
		padding: 30rpx;
	}
	/* 设备区域样式 */
	.dev-area {
		display: flex;
		/* 两端对齐 */
		justify-content: space-between;
		/* 自动换行 */
		flex-wrap: wrap;
	}
	/* 设备卡片样式 */
	.dev-card {
		height: 150rpx;
		width: 320rpx;
		/* 边框圆角30像素 */
		border-radius: 30rpx;
		/* 上外边距30像素 */
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		/* 卡片阴影灰色 */
		box-shadow: 0 0 15rpx #ccc;
	}
	
	.dev-card-1{
		height: 150rpx;
		width: 700rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.dev-name {
		font-size: 20rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.dev-data {
		font-size: 50rpx;
		color: #6d6d6d;
	}

	.dev-logo {
		height: 70rpx;
		width: 70rpx;
		margin-top: 12rpx;
	}
	
	.ctrl-slider{
		width: 580rpx;
	}
</style>