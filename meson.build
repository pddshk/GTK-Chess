project(
    'GTKChess',
    'c',
    version: '0.0.1',
    license: 'WTFPL',
    default_options: [
        'c_std=c18',
        'warning_level=3',
        'optimization=3'
    ]
)

add_project_arguments('-Wno-overlength-strings', '-I../include', language: 'c')

dep_gtk3 = dependency('gtk+-3.0', version: '>= 3.24.0')
dep_glib = dependency('glib-2.0', version: '>= 2.64.0')
dep_giounix = dependency('gio-unix-2.0', version: '>= 2.64.0')
dep_rsvg = dependency('librsvg-2.0', version: '>= 2.55.0')

subdir('data')

gnome = import('gnome')
gresources = gnome.compile_resources(
    'UI',
    gresources_file,
    source_dir: 'data'
)

subdir('src')
executable(
    'engine_manager',
    engine_manager_sources,
    dependencies: [dep_giounix],
    link_args: ['-pthread'],
    install: true,
    install_dir: '.'
)

executable(
    'GTKChess',
    gtkchess_sources,
    gresources,
    dependencies: [dep_rsvg, dep_giounix, dep_glib, dep_gtk3],
    link_args: ['-rdynamic'],
    install: true,
    install_dir: '.'
)