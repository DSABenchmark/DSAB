var url = "http://188.131.137.105:8086/skbm/api";

$.get(url+'?'+'get=datasetList', function(lst) {
	for (var i in lst){
		item = lst[i];
		$('#dataset-dropdown')
			.prepend('<a class="dropdown-item" href="#">'+item+'</a>');
	}
	for (var i in lst) {
		var item = $('#dataset-dropdown a').get(i);
		$(item).on("click", function(){
				// console.log(this);
				$('#dataset-row .btn').text($(this).text());
			});
	}
},dataType='json');

$.get(url+'?'+'get=sketchList', function(lst) {
	for (var i in lst){
		item = lst[i];
		$('#sketch-dropdown')
			.prepend('<a class="dropdown-item" href="#">'+item+'</a>');
	}
	for (var i in lst) {
		var item = $('#sketch-dropdown a').get(i);
		$(item).on("click", function(){
				var t = $(this).text();
				$('#sketch-row .btn').text(t);
				getTasks(t);
			});
	}
},dataType='json');

function getTasks(t) {
	$.get(url+'?'+'get=taskList&sketch='+t, function(dct){
		var lst = JSON.parse(dct).tasks;
		console.log(dct);
		console.log(lst);
		for (var i in lst){
			item = lst[i];
			$('#task-dropdown')
				.prepend('<a class="dropdown-item" href="#">'+item+'</a>');
		}
		for (var i in lst) {
			var item = $('#task-dropdown a').get(i);
			$(item).on("click", function(){
					// console.log(this);
					var task = $(this).text();
					$('#task-row .btn').text(task);
					insertArgs(t,task);
				});
		}
	});
}

function insertArgs(sketch, task) {
	$.get(url+'?'+'get=args&sketch='+sketch+'&task='+task, function(dct) {
		// console.log(dct);
		dct = JSON.parse(dct);
		lst = dct['params'];
		for (var i in lst){
			arg = lst[i];
			argName = arg.field;
			argType = arg.type;
			console.log(argName+argType);
			insertArgument(argName, argType);
		}
	});
}

function insertArgument(argName, argType) {

}
















