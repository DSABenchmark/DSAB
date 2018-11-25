(function() {
'use strict';

angular.module('SketchApp', [])
.controller('datasetController', datasetController)
.controller('expController', expController)
.controller('submitController', submitController)
.controller('graphController', graphController)
.controller('graphController2', graphController2)
.service('metaService', metaService)
.service('requestService', requestService)
.constant('baseURL', "http://"+document.location.host+"/skbm/api")
.component('loadingSpinner', {
  template: '<img ng-if="$ctrl.showSpinner" class="loading-icon" src="../static/flickr-loading.gif" alt="loading">',
  controller: SpinnerController
});

SpinnerController.$inject = ['$rootScope']
function SpinnerController($rootScope) {
  var $ctrl = this;

  var cancelListener = $rootScope.$on('experiments:running', function (event, data) {
    console.log("Event: ", event);
    console.log("Data: ", data);

    if (data.on) {
      $ctrl.showSpinner = true;
    }
    else {
      $ctrl.showSpinner = false;
    }
  });

  $ctrl.$onDestroy = function () {
    cancelListener();
  };

};

datasetController.$inject = ['requestService','metaService'];
function datasetController(requestService, metaService){
	var dsc = this;

	var promise = requestService.requestList('datasetList');
	promise.then(function(response){
		metaService.setDatasetList(response.data);
		dsc.datasetList = metaService.getDatasetList();
		console.log(dsc.datasetList);
		$(function(){
			$('[data-toggle="popover"]').popover()
		});
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

function deepCopy(obj){
    if(typeof obj != 'object'){
        return obj;
    }
    var newobj = {};
    for ( var attr in obj) {
    	if(attr!=="$$hashKey")
        	newobj[attr] = deepCopy(obj[attr]);
    }
    return newobj;
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
		var L = ec.experimentList.length;
		var lastExperiment = ec.experimentList[L-1];
		console.log(lastExperiment);
		ec.experimentList.push(deepCopy(lastExperiment));
		// ec.experimentList.push({
		// 	sketchName: "Sketch",
		// 	taskName: "Task"
		// });
	}
}

submitController.$inject = ['requestService','metaService', '$rootScope'];
function submitController(requestService,metaService, $rootScope) {
	var sbc = this;

	sbc.submitExperiments = function(){
		$rootScope.$broadcast('experiments:running', {on: true});

		sbc.datasetList = metaService.getDatasetList();
		sbc.experimentList = metaService.getExperimentList();
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
		promise
		.then(function(response){
			metaService.setResult(response.data);
		})
		.catch(function(error){
			console.log(error);
		})
		.finally(function () {
			$rootScope.$broadcast('experiments:running', {on: false});
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
			gc.graphLink = "http://"+document.location.host+"/skbm/graph?uuid="+response.data;
		}).catch(function(error){
			console.log(error);
		});
	}
}

graphController2.$inject = ['requestService','metaService'];
function graphController2(requestService,metaService){
	var gc = this;

	gc.chosen_yaxis = "";
	gc.xlabel = "";

	gc.result = metaService.getResult();

	gc.pointList = [
		{
			"line": "",
			"index": "",
			"experimentIdx": "",
		}
	];

	gc.num_points = "1";
	gc.num_lines = "1";

	gc.search = "";

	gc.addPoint = function() {
		var L = gc.pointList.length;
		gc.pointList.push(
			{
				"line": gc.pointList[L-1]["line"],
				"index": gc.pointList[L-1]["index"],
				"experimentIdx": gc.pointList[L-1]["experimentIdx"],
			}
		);
	};

	gc.deletePoint = function() {
		console.log("hello");
		gc.pointList.splice(gc.pointList.length-1,1);
	};

	gc.draw = function() {
		var d = {
			"flag": "graph2",
			"pointList": gc.pointList,
			"results": gc.result.results,
			"yaxis": gc.chosen_yaxis,
			"xlabel": gc.xlabel
		};
		var promise = requestService.postGraph2(d);
		promise.then(function(response){
			gc.graphLink = "http://"+document.location.host+"/skbm/graph?uuid="+response.data;
		}).catch(function(error){
			console.log(error);
		});
	};
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
	};

	service.postGraph = function(d) {
		var response = $http({
			method: 'POST',
			url: baseURL,
			data: d
		});
		return response;
	};

	service.postGraph2 = function(d) {
		var response = $http({
			method: "POST",
			url: baseURL,
			data: d
		});
		return response;
	};
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


















