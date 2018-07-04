///
/// Called when the page is loaded - find out and set the state of each light
///
function on_page_load() {
    var success = function(data) {
        var json_data = JSON.parse(data);
        for (l in json_data['lights']) {
            create_light_entry(l, lights[l])
        }
    }

    $.ajax({
      url: '/universe_state/',
      type: "GET",
      success: success
    })
}

///
/// Get a list of light property values using the id of the light
///
function get_state_of_light(light_id) {
    var this_light = document.getElementById(light_id);
    var this_light_value_elements = this_light.getElementsByClassName('light_slider');

    // Now we've got an ordered set of sliders that we should push together into a value list
    var values = []
    for (var i = 0; i < this_light_value_elements.length; i++) {
        var element = this_light_value_elements[i];
        values.push(element.value)
    }

    return values
}

///
/// Update a single light state, this can be faster than updating the whole universe state
///
function update_single_light(light_id)  {
    var data_to_send = JSON.stringify(get_state_of_light(light_id));
    $.ajax({
      url: '/universe_state/set_light/' + light_id,
      type: "POST",
      data: data_to_send,
      contentType: "application/json; charset=utf-8",
      dataType: "json"
    })
}

///
/// Update all the lights in this universe, send one big batch update request
///
function update_lights(light_ids)  {
    var light_entries = document.getElementById("light_entries").getElementsByClassName("light_entry");

    var light_values = {};
    for (var i = 0; i < light_entries.length; i++) {
        var light_entry = light_entries[i];

        console.log(light_entry.id)
        light_values[light_entry.id] = get_state_of_light(light_entry.id);
    }

    var data_to_send = JSON.stringify(light_values);
    console.log(data_to_send)

    $.ajax({
      url: '/universe_state/set_lights',
      type: "POST",
      data: data_to_send,
      contentType: "application/json; charset=utf-8",
      dataType: "json"
    })
}

///
/// Called when any light property is change. This will check if the live_updates check box is checked and
/// update the single light if so
///
function update_single_light_live(light_id) {
    // Don't write if we're not supposed to do live updates
    if ($("#do_live_updates").is(":checked")) {
        update_single_light(light_id);
    }
}

///
/// Create an all new light entry and insert it into the right spot
///
function create_light_entry(light_id, light_attrs) {
    // Start building the DOM element that will be inserted.
    var light_entry = $('#light_entry_template').clone();
    light_entry.find('.light_entry')
               .attr('id', light_id)
               .find('.light_entry_title')
               .text(light_attrs['name']);

    // Add a slider for each property, make it so it'll update the state on change
    for (p in light_attrs['property_keys']) {
        var property_name = light_attrs['property_keys'][p];
        var property_value = light_attrs['property_values'][p];

        var this_slider = $('#light_entry_slider_holder_template').clone();
        this_slider.find('.light_entry_slider_title')
                   .text(property_name + ':');

        this_slider.find('.light_slider')
                   .attr('id', property_name)
                   .attr('value', property_value)
                   .attr('oninput', 'update_single_light(' + light_id + ')');

        light_entry.find('.light_entry_sliders_holder')
                   .append(this_slider.html());
    }

    $("#light_entries").append(light_entry.html());
}
