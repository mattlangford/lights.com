///
/// Called when the page is loaded - find out and set the state of each light
///
function on_page_load() {
    $("#light_entries").text("");

    var success = function(data) {
        console.log(data)
        for (l in data.lights) {
            create_light_entry(l, data.lights[l])
        }
        for (s in data.scenes) {
            add_scene(data.scenes[s])
        }
    }

    $.ajax({
      url: '/universe_state',
      type: "GET",
      success: success,
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
        values.push(parseFloat(element.value));
    }

    console.log(values)
    return values
}

///
/// Update a single light state, this can be faster than updating the whole universe state
///
function update_single_light(light_id)  {
    var light_data = {};
    light_data[light_id] = get_state_of_light(light_id);
    var data_to_send = {lights: light_data, transition_type: "direct_fade"};

    console.log(data_to_send)
    $.ajax({
      url: '/universe_state',
      type: "POST",
      data: JSON.stringify(data_to_send),
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

        light_values[light_entry.id] = get_state_of_light(light_entry.id);
    }

    var data_to_send = {lights: light_values,
                        transition_type: "exponential_fade",
                        transition_duration: parseFloat(document.getElementById("fade_duration_slider").value)};
    console.log(data_to_send)

    $.ajax({
      url: '/universe_state',
      type: "POST",
      data: JSON.stringify(data_to_send),
      contentType: "application/json; charset=utf-8",
      dataType: "text"
    })
}

///
/// Called when any light property is change. This will check if the live_updates check box is checked and
/// update the single light if so
///
function update_single_light_live(light_id) {
    if ($("#do_live_updates").is(":checked")) {
        update_single_light(light_id);
    }
}

///
/// Update the scene that's currently running
///
function update_scene(scene_name)  {
    var data_to_send = {scene: scene_name};

    console.log(JSON.stringify(data_to_send))
    $.ajax({
          url: '/universe_state',
          type: "POST",
          data: JSON.stringify(data_to_send),
          contentType: "application/json; charset=utf-8",
          dataType: "text"
    }).done(function(response){
          console.log('success');
    }).fail(function(jqXHR, textStatus, errorThrown){
          console.log('FAILED! ERROR: ' + errorThrown);
    });
}

///
/// Adds an entry to the scenes selector
///
function add_scene(scene_name) {
    var scene_entry = $('#scene_selector_option_template').clone();
    scene_entry.find('.scene_selector_option')
               .attr('onclick', 'update_scene("' + scene_name + '")')
               .text(scene_name);

    $('#scene_selector').append(scene_entry.html());
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
               .text(light_attrs.light_name);

    // Add a slider for each channel, make it so it'll update the state on change
    for (c in light_attrs.channels) {
        var channel = light_attrs.channels[c];

        var this_slider = $('#light_entry_slider_holder_template').clone();
        this_slider.find('.light_entry_slider_title')
                   .text(channel.name + ':');

        this_slider.find('.light_slider')
                   .attr('id', channel.name)
                   .attr('min', channel.min_value)
                   .attr('max', channel.max_value)
                   .attr('level', channel.level)
                   .attr('oninput', 'update_single_light_live(' + light_id + ')');

        light_entry.find('.light_entry_sliders_holder')
                   .append(this_slider.html());
    }

    $("#light_entries").append(light_entry.html());
}
