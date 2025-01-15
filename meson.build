project('frostwm', 'c', version: '0.1.0', default_options: ['warning_level=3'])

add_project_arguments(
    [
        '-DWLR_USE_UNSTABLE', # required for certain wlroots functions
        '-fmacro-prefix-map=../src/=', # prettifies filenames for logging

        # warning supressions
        '-Wno-unused-parameter',
    ],
    language: 'c',
)

if get_option('buildtype') == 'debug'
    debug()
    add_project_arguments('-DFROST_DEBUG', language: 'c')
endif

wayland = dependency('wayland-server')
wlroots = dependency('wlroots-0.18')
xkb = dependency('xkbcommon')

subdir('src')

executable(
    'frostwm',
    sources: sources,
    include_directories: include_directories('include'),
    dependencies: [wayland, wlroots, xkb],
)