var gulp = require('gulp');
var spawn = require('child_process').spawn;

var running = false;

gulp.task('watch', function() {
  gulp.watch(['src/*', 'Makefile'], function() {
    if (!running) {
      running = true;
      var cmd = spawn('make', ['debug'], {stdio: 'inherit'});
      cmd.on('close', function (code) {
        running = false;
      });
    }
  });
});

