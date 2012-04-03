<?php

/**
 * @author Cédric Cornez http://drupal.org/user/316578
 * @file
 * Use Google Charts 2 on your site.
 */
function _google_charts_2($type) {
    $types = array(
        'line2D' => array('class' => 'LineChart', 'attributes_per_value' => FALSE),
        'hbar2D' => array('class' => 'BarChart', 'attributes_per_value' => FALSE),
        'vbar2D' => array('class' => 'ColumnChart', 'attributes_per_value' => FALSE),
        'hbar2Ds' => array('class' => 'BarChart', 'attributes_per_value' => FALSE,
            'options' => array('isStacked' => TRUE)),
        'vbar2Ds' => array('class' => 'ColumnChart', 'attributes_per_value' => FALSE,
            'options' => array('isStacked' => TRUE)),
        'pie2D' => array('class' => 'PieChart', 'attributes_per_value' => TRUE),
        'pie3D' => array('class' => 'PieChart', 'attributes_per_value' => TRUE,
            'options' => array('is3D' => TRUE)),
            //'scatter' => array('class' => 'ScatterChart', 'attributes_per_value' => FALSE),
    );
    return $types[$type];
}

/**
 * Implements hook_charts_render().
 *
 * Its a Charts module hook. It transform the data into a HTML chart.
 *
 * @param &$data
 *   Array. The
 */
function _google_charts_2_render(&$data) {
    drupal_add_js('https://www.google.com/jsapi', 'external');

    if (isset($data['#chart_id']))
        $chart_id = $data['#chart_id'];
    else
        $chart_id = md5(rand(0, 999999999999));

    if (!empty($data['#title']))
        $title = ", title: '" . $data['#title'] . "'";

    if (empty($data['#width']) or empty($data['#height'])) {
        return t('Height and Width are required');
    }
    $width = $data['#width'];
    $height = $data['#height'];

    $chart_info = _google_charts_2($data['#type']);
    $chart_class = $chart_info['class'];

    // get default chart options
    if (!empty($chart_info['options']))
        $options = $chart_info['options'];
    else
        $options = array();

    // add an option to hide the legend
    if (!$data['#legend']) {
        $options['legend'] = "'none'";
    }

    // add an option to display labels
    if ($data['#label']) {
        if ($chart_class == 'PieChart')
            $options['pieSliceText'] = "'label'";
    }

    // add the option for the background
    if (!empty($data['#color']['background']) and drupal_strtolower($data['#color']['background']) != '#ffffff') {
        $options['backgroundColor'] = "'" . $data['#color']['background'] . "'";
    }
    
    $chart_js = "google.load('visualization', '1', {packages:['corechart']});\n";
    $chart_js .= "google.setOnLoadCallback(drawChart);\n";
    $chart_js .= "function drawChart() {\n";
    $chart_js .= "var data = new google.visualization.DataTable();\n";
    $chart_js .= "var chart = new google.visualization.$chart_class(document.getElementById('$chart_id'));\n";


    $colors = array();
    $chart_js .= _google_charts_2_series($data, $colors);

    // add option to use given colors
    if (count($colors)) {
        $options['colors'] = '[' . implode(', ', $colors) . ']';
    }

    // convert options into js
    foreach ($options as $key => $value) {
        $chart_options .= ", $key: $value";
    }


    $chart_js .= "chart.draw(data, {
	    width: $width,
	    height: $height
	    $chart_options
	});\n";
    $chart_js .= '}';

    drupal_add_js($chart_js, array('type' => 'inline', 'group' => JS_THEME));

//    if (isset($_GET['showjs'])) {
//        $debug = print_r($data, TRUE) . '<hr/>';
//        $debug .= str_replace("\n", "<br />", check_plain($chart_js));
//    }

    $width = $width . 'px';

    //return "<div id='$chart_id' style='width: $width;'></div>" . $debug;
    return "<div id='$chart_id' style='width: $width;'></div>";
}

/**
 * Convert all Series-level data.
 *
 * @param &$chart
 *   Array. The array that will be converted into a string for Google Charts
 * @param &$data
 *   Array. The raw data.
 * @return
 *   String. The string presentation of series data
 */
function _google_charts_2_series(&$data, &$colors) {
    $options = _google_charts_2($data['#type']);

    $ans = '';

    $value_labels = array();

    // For each chart value, encode it
    // Note: Underscore represents a missing value
    $serie_index = 0;

    $ans .= "data.addColumn('string');\n";

    foreach (element_children($data) as $series) {
        if (!empty($data[$series]['#legend']))
            $ans .= "data.addColumn('number', '" . $data[$series]['#legend'] . "');\n";
        else
            $ans .= "data.addColumn('number');\n";
    }

    $rows = array();
    $labels = array();
    foreach (element_children($data) as $series) {
        $rows[] = _charts_series_values($data[$series]);

        $labels[] = "'" . $data[$series]['#legend'] . "'";
    }

    // create header & colors
    $header = array();
    foreach (element_children($data[0]) as $item) {
        if (!empty($data[0][$item]['#label']))
            $header[] = $data[0][$item]['#label'];
        if (!empty($data[0][$item]['#color']))
            $colors[] = "'" . $data[0][$item]['#color'] . "'";
    }

    for ($i = 0; $i < count($rows[0]); $i++) {
        $currow = array();
        for ($j = 0; $j < count($rows); $j++) {
            $currow[] = $rows[$j][$i];
        }
        $ans .= "data.addRow([";
        $ans .= "'$header[$i]',";
        $ans .= implode(',', $currow);
        $ans .= "]);\n";
    }

    return $ans;
}
