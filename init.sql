use hw_mto10;

drop table if exists student;
create table student
(
    stu_no char(7) primary key not null,
    stu_name char(8) not null,
    stu_password char(32) not null,
    stu_common_password char(32) not null,
    stu_enable char(1) default "1"
);

--stu_common_password明文为"123"
insert into student values("1234567","张三","b2d4a21f19402e204ba5a2a7d3d35a75","202cb962ac59075b964b07152d234b70",1);
insert into student values("1751396","sad","e95f76e874261eb582c287f588ea7612","202cb962ac59075b964b07152d234b70",1);

drop table if exists game;
create table game
(
    game_id int primary key auto_increment,
    game_time datetime not null,
    stu_no char(7) not null,
    game_mode char(1) not null,
    game_mapid int,
    game_row tinyint,
    game_col tinyint,
    game_score int not null,
    game_step int not null,
    game_maxvalue tinyint not null,
    game_time_consumed int not null,
    game_result char(20) not null,
    game_weighted_score decimal(8,4)
);

drop table if exists competiiton_map;
create table competiiton_map
(
    map_id int primary key auto_increment,
    time_start datetime,
    duration_minute smallint,
    seed int,
    row tinyint,
    col tinyint
);