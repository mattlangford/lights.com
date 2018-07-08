# dmx_control
Arbitrates lights over DMX, provides functions to facilitate that and a server to do it over ethernet

# Building
`bazel build //... --cxxopt='-std=c++1z'`
Wow that's easy!

# HTTP Server
There's an http server that can be hosted and control lights. Here is the API for that:

`GET /universe_state`
returns something like
`{lights: {<light_id>: {light_name: "name", channel_names: ["red", "green", "blue"], channel_values: [233, 123, 0]}, <light_id2>: {...}}}`

`POST /universe_state <command>`
Can be used to set the state of the whole universe it accepts a command like
`{lights: {<light_id>: [23, 43, 100], <light_id2>: [122, 100, 99], ...}}`
where any number of lights can be set. The list should be the same length as the `channel_values` where each index maps to one channel.
