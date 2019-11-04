const {task} = require('gulp');
let gulp = require('gulp');
let concat = require('gulp-concat');
let cleanCSS = require('gulp-clean-css');
let uglify = require('gulp-uglify');

gulp.task('css', () => {
  return gulp.src(['./assets_src/css/milligram.css', './assets_src/css/nouislider.css', './assets_src/css/main.css'])
    .pipe(concat('main.min.css'))
    .pipe(cleanCSS())
    .pipe(gulp.dest('./data/assets/css/'));
});

gulp.task('js', () => {
  return gulp.src(['./assets_src/js/doT.js', './assets_src/js/iro.js', './assets_src/js/wNumb.js', './assets_src/js/nouislider.js', './assets_src/js/main.js'])
    .pipe(uglify())
    .pipe(concat('main.min.js'))
    .pipe(gulp.dest('./data/assets/js/'));
});

const css = task('css');
const js = task('js');