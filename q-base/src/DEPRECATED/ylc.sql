-- Yoni List Creator
drop table if exists flds_x_xlist; 
drop table if exists xlist; 

create table xlist ( -- list of exclusions
    id integer PRIMARY KEY ASC,
    cum_fld varchar(32), -- redundant because of cum_fld_id 
    list varchar(32), -- redundant because of list_id 
    list_id int(11), -- redundant because of cum_fld_id 
    cum_fld_id int(11) not null,
    FOREIGN KEY (list_id) REFERENCES list(id)
    );

create table flds_x_xlist (
    id integer PRIMARY KEY ASC,

    fld varchar(32), -- redundant because of fld_id 
    fld_id int(11) not null, 
    xlist_id int(11) not null, 
    FOREIGN KEY (xlist_id) REFERENCES xlist(id)
    );

drop table if exists flds_x_list; 
drop table if exists list; 
create table list (
    id integer PRIMARY KEY ASC,
    tbl_id int(11) not null,
    tbl varchar(32), -- redundant because of tbl_id 
    xlist_id int(11) unique, -- a list may or may not have an exclusion list
    name varchar(256) unique,
    FOREIGN KEY (xlist_id) REFERENCES xlist(id)
    );

create table flds_x_list (
    id integer PRIMARY KEY ASC,

    fld varchar(32) not null, -- redundant because of fld_id 
    fld_id int(11) not null, 
    fld_cnt big int not null, 

    new_fld varchar(32) not null, -- redundant because of new_fld_id 
    new_fld_id int(11) not null,  -- position = 1 <=> new_fld_id = fld_id
    new_fld_cnt big int not null, 

    cum_fld varchar(32), -- redundant because of cum_fld_id 
    cum_fld_id int(11),
    cum_fld_cnt big int,

    xcl_fld varchar(32) not null, -- redundant because of xcl_fld_id 
    xcl_fld_id int(11) not null,  -- 
    xcl_fld_cnt big int not null, 

    lmt_fld varchar(32) not null, -- redundant because of lmt_fld_id 
    lmt_fld_id int(11) not null,  
    lmt_fld_cnt big int not null, 

    list_id int(11) not null, 
    position int(11) not null , -- for given list_id, position = 1, 2, ... 
    FOREIGN KEY (list_id) REFERENCES list(id)
    );

-- insert into xlist (id, cum_fld, cum_fld_id, cum_fld_cnt) values (1, "cum_fld", -2, '999999');
-- insert into list (name, tbl_id, tbl, xlist_id) values ('BOGUS_LIST', -1, 'Big Table', 1);
-- insert into flds_x_list (fld, fld_id, new_fld, new_fld_id, lmt_fld, lmt_fld_id, list_id, position, fld_cnt) 
--   values ("fld", 777, "newfld", 888, "lmtfld", 999, 1, 1, -1);
-- select * from xlist;
-- select * from list;
-- select * from flds_x_list;

