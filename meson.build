project(
  'frostwm',
  'cpp',
  default_options: ['warning_level=3', 'cpp_std=c++20'],
)

add_project_arguments(
  [
    '-DWLR_USE_UNSTABLE',
    '-fmacro-prefix-map=../=',
  ],
  language: 'cpp',
)

wayland = dependency('wayland-server')
wlroots = dependency('wlroots-0.19')

executable(
  'frostwm',
  sources: files('src/main.cpp', 'src/session.cpp'),
  dependencies: [wayland, wlroots],
  include_directories: include_directories('include'),
)
