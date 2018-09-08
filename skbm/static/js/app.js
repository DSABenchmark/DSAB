(function() {
'use strict';

angular.module('SketchApp', [])
.controller('datasetController', datasetController)
.controller('expController', expController)
.controller('submitController', submitController)
.controller('graphController', graphController)
.service('metaService', metaService)
.service('requestService', requestService)
.constant('baseURL', "http://188.131.137.105:8086/skbm/api");

datasetController.$inject = ['requestService','metaService'];
function datasetController(requestService, metaService){
	var dsc = this;

	var promise = requestService.requestList('datasetList');
	promise.then(function(response){
		metaService.setDatasetList(response.data);
		dsc.datasetList = metaService.getDatasetList();
	})
	.catch(function(error){
		console.log('Error when requesting datasetList');
	});

	dsc.choose = function(idx) {
		if(dsc.datasetList[idx].state){
			dsc.datasetList[idx].state = '';
		}
		else {
			dsc.datasetList[idx].state = 'active';
		}
	};
}

expController.$inject = ['requestService','metaService'];
function expController(requestService,metaService) {
	var ec = this;

	var promise = requestService.requestList('sketchList');
	promise.then(function(response){
		metaService.setSketchList(response.data);
		ec.sketchList = metaService.getSketchList();
	})
	.catch(function(error){
		console.log('Error when requesting sketchList');
	});

	ec.experimentList = metaService.getExperimentList();

	ec.writeSketchName = function(sketchIdx, experimentIdx) {
		ec.experimentList[experimentIdx].sketchName = ec.sketchList[sketchIdx].name;
		ec.experimentList[experimentIdx].chosenSketch = ec.sketchList[sketchIdx];
	};
	ec.writeTaskName = function(name, experimentIdx) {
		ec.experimentList[experimentIdx].taskName = name;
		var task = ec.experimentList[experimentIdx].chosenSketch.tasks.find(function(task){return name==task.name;})
		ec.experimentList[experimentIdx].params = ec.experimentList[experimentIdx].chosenSketch.params.concat(task.params);
		for(var i in ec.experimentList[experimentIdx].params){
			var param = ec.experimentList[experimentIdx].params[i];
			param.from = "";
			param.to = "";
			param.step = "";
		}
	};

	ec.addExperiment = function(){
		ec.experimentList.push({
			sketchName: "Sketch",
			taskName: "Task"
		});
	}
}

submitController.$inject = ['requestService','metaService'];
function submitController(requestService,metaService) {
	var sbc = this;

	sbc.submitExperiments = function(){
		sbc.datasetList = metaService.getDatasetList();
		sbc.experimentList = metaService.getExperimentList();
		// get all the experiments
		// console.log(sbc.datasetList);
		// console.log(sbc.experimentList);
		// var experiments = []
		// for(var i in sbc.datasetList){
		// 	var dataset = sbc.datasetList[i];
		// 	if(dataset.state){
		// 		console.log(dataset.name);
		// 		for(var j in sbc.experimentList){
		// 			var row = sbc.experimentList[j];
		// 			var params = row.params;
		// 			var steps = [];
		// 			var grids = []
		// 			var totSteps = 1;
		// 			for(var k=0;k<params.length;k++){
		// 				steps.push(0);
		// 				var param = params[k];
		// 				var tmp = parseInt((param.to-param.from)/param.step);
		// 				totSteps *= tmp+1;
		// 				grids.push(tmp);
		// 			}
		// 			while(totSteps>0){
		// 				totSteps--;
		// 				var arg = {
		// 							datasetName: dataset.name,
		// 							sketchName: row.sketchName,
		// 							taskName: row.taskName
		// 						};
		// 				for(var s in steps){
		// 					var param = params[s];
		// 					var v = steps[s] * param.step + param.from;
		// 					arg[param.field] = v;
		// 				}
		// 				experiments.push(arg);
		// 				incrementSteps(steps,grids);
		// 			}

					
		// 		}
		// 	}
		// }
		// console.log(experiments);
		var dsList = [];
		for(var i in sbc.datasetList) {
			var dataset = sbc.datasetList[i];
			if(dataset.state) dsList.push(dataset.name);
		}
		// console.log(dsList);
		var d = {
			'flag': "experiment",
			'datasetList': dsList,
			'experimentList': sbc.experimentList
		};
		var promise = requestService.postExperiments(d);
		promise.then(function(response){
			metaService.setResult(response.data);
		})
		.catch(function(error){
			console.log(error);
		});
	};
}

graphController.$inject = ['requestService','metaService'];
function graphController(requestService,metaService){
	var gc = this;

	gc.result = metaService.getResult();

	gc.chosen_yaxis = "";
	gc.chosen_xaxis = "";
	gc.chosen_multilines = "";

	gc.graphLink = "";

	gc.showSelection = function() {
		console.log(gc.chosen_yaxis+" "+gc.chosen_xaxis+" "+gc.chosen_multilines);
	};
	gc.draw = function() {
		var d = {
			"flag": "graph",
			"results": gc.result.results,
			"yaxis": gc.chosen_yaxis,
			"xaxis": gc.chosen_xaxis,
			"multilines": gc.chosen_multilines
		};
		var promise = requestService.postGraph(d);
		promise.then(function(response){
			gc.graphLink = "http://188.131.137.105:8086/skbm/graph?uuid="+response.data;
		}).catch(function(error){
			console.log(error);
		});
	}
}


function metaService() {
	var service = this;

	var datasetList = [];
	var sketchList = [];
	var experimentList = [
		{
			sketchName: "Sketch",
			taskName: "Task"
		}
	];
	var result = {};
	service.getDatasetList = function(){return datasetList};
	service.getSketchList = function(){return sketchList};
	service.getExperimentList = function(){return experimentList};
	service.getResult = function(){return result};
	service.setDatasetList = function(data){datasetList = data;};
	service.setSketchList = function(data){sketchList = data;};
	service.setResult = function(data){
		for(var key in data){
			result[key] = data[key];
		}
		console.log(result);
	};
}

requestService.$inject = ['$http','baseURL'];
function requestService($http,baseURL) {
	var service = this;

	service.requestList = function(listname) {
		var response = $http({
			method: 'GET',
			url: baseURL,
			params: {
				get: listname
			}
		});
		return response
	};

	service.postExperiments = function(d) {
		var response = $http({
			method: 'POST',
			url: baseURL,
			data: d
		});
		return response;
	}

	service.postGraph = function(d) {
		var response = $http({
			method: 'POST',
			url: baseURL,
			data: d
		});
		return response;
	}
}

function incrementSteps(steps,grids){
	for(var i in steps){
		var step = steps[i];
		var grid = grids[i];
		if(step < grid){
			steps[i]++;
			return;
		}
		else {
			continue;
		}
	}
}


})();


















