BEGIN;

CREATE TABLE ig_system (
    system_id        VARCHAR(512) PRIMARY KEY,     -- 预留足够多的空间
    main_id          VARCHAR(255) NOT NULL,        -- Objects根键MainID
    source_file      VARCHAR(1024),                -- 原始.sc路径
    source_hash      CHAR(64),                     -- 文件sha256，决定要不要重新导入
    code_page        INTEGER,
    esi_estimator    VARCHAR(32),                  -- SolarSys/Extrasolar
    absolute_orbit   BOOLEAN NOT NULL DEFAULT FALSE,
    common_plane_threshold INTEGER,
    namespace        VARCHAR(255) NOT NULL,        -- 命名空间，由用户填写（查询时也由用户指定而不是AI查找）
    n_stars          INTEGER,
    n_planets        INTEGER,
    n_dwarf_planets  INTEGER,
    n_satellites     INTEGER,
    n_minor_planets  INTEGER,
    n_comets         INTEGER,
    spectral_types   TEXT,                         -- StarSpectralType（‘+’号分隔）
    create_date      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    modified_date    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (main_id, source_hash)
);

CREATE TABLE ig_object (
    object_id        VARCHAR(512) PRIMARY KEY,
    system_id        VARCHAR(512) NOT NULL REFERENCES ig_system (system_id) ON DELETE CASCADE,
    parent_object_id VARCHAR(512), --REFERENCES ig_object (object_id) ON DELETE CASCADE,  -- 根节点为NULL
    primary_name     VARCHAR(255) NOT NULL,        -- Identifiers[0]，即Objects的键
    otype            VARCHAR(32)  NOT NULL,        -- Barycenter/Star/Planet/DwarfPlanet/Moon/DwarfMoon/Asteroid/Comet
    class            VARCHAR(64),                  -- PhysicalCharacteristics.Class，热过滤列（Terra/Jupiter/...）
    depth            INTEGER NOT NULL DEFAULT 0,   -- 根为0
    sibling_index    INTEGER,                      -- SubSystems 内序号，保留 --sort-system 的顺序
    is_minor         BOOLEAN NOT NULL DEFAULT FALSE,
    UNIQUE (system_id, parent_object_id, primary_name)
);
-- CREATE INDEX ix_ig_object_name  ON ig_object (system_id, primary_name);
-- CREATE INDEX ix_ig_object_otype ON ig_object (otype);
-- CREATE INDEX ix_ig_object_class ON ig_object (class);

CREATE TABLE ig_identifiers (
    object_id VARCHAR(512) NOT NULL REFERENCES ig_object (object_id) ON DELETE CASCADE,
    alias     VARCHAR(255) NOT NULL,
    PRIMARY KEY (object_id, alias)
);
-- CREATE INDEX ix_ig_identifiers_alias ON ig_identifiers (alias);

CREATE TABLE ig_physical ( -- 部分字段只有特定类型的物体才有，不过这个只需要在查询时按目标物体类型分流就行
    object_id                    VARCHAR(512) PRIMARY KEY REFERENCES ig_object (object_id) ON DELETE CASCADE,
    abs_magn_bol                 DOUBLE PRECISION,
    mean_radius                  DOUBLE PRECISION,  -- m
    dimension_x                  DOUBLE PRECISION,  -- Dimensions[0..2]，m
    dimension_y                  DOUBLE PRECISION,
    dimension_z                  DOUBLE PRECISION,
    flattening_x                 DOUBLE PRECISION,
    flattening_y                 DOUBLE PRECISION,
    flattening_z                 DOUBLE PRECISION,
    circumference_equatorial     DOUBLE PRECISION,  -- Circumference[0], m
    circumference_meridional     DOUBLE PRECISION,  -- Circumference[1]，m
    surface_area                 DOUBLE PRECISION,  -- m^2
    volume                       DOUBLE PRECISION,  -- m^3
    mass                         DOUBLE PRECISION,  -- kg
    mean_density                 DOUBLE PRECISION,  -- kg/m^3
    age                          DOUBLE PRECISION,  -- yr，仅恒星
    surface_gravity              DOUBLE PRECISION,  -- m/s^2
    moment_of_inertia_factor     DOUBLE PRECISION,
    escape_velocity              DOUBLE PRECISION,  -- m/s
    synodic_rotation_period      DOUBLE PRECISION,  -- s
    sidereal_rotation_period     DOUBLE PRECISION,  -- s
    equatorial_rotation_velocity DOUBLE PRECISION,  -- m/s
    axial_tilt                   DOUBLE PRECISION,  -- deg
    albedo_bond                  DOUBLE PRECISION,  -- Albedo[0]
    albedo_geometric             DOUBLE PRECISION,  -- Albedo[1]
    luminosity                   DOUBLE PRECISION,  -- W，仅恒星
    temperature                  DOUBLE PRECISION,  -- K
    radiant_flux                 DOUBLE PRECISION,  -- W/m^2
    kerr_spin                    DOUBLE PRECISION,  -- 仅黑洞
    kerr_charge                  DOUBLE PRECISION,  -- 仅黑洞
    comet_total_magn             DOUBLE PRECISION,  -- 仅彗星
    comet_total_magn_slope       DOUBLE PRECISION,  -- 仅彗星
    esi                          DOUBLE PRECISION,  -- 仅行星/卫星，C++侧已算好，用于宜居度评级（但不是唯一因素）
    extra                        TEXT               -- 兜底：未单独建列的字段（JSON）
);
-- CREATE INDEX ix_ig_physical_temp   ON ig_physical (temperature);
-- CREATE INDEX ix_ig_physical_mass   ON ig_physical (mass);
-- CREATE INDEX ix_ig_physical_radius ON ig_physical (mean_radius);
-- CREATE INDEX ix_ig_physical_esi    ON ig_physical (esi);

CREATE TABLE ig_orbit ( -- 保存所有的中间数据，但是查询时也要根据物体类型分流，即单条语句可能会非常复杂
    object_id              VARCHAR(512) PRIMARY KEY REFERENCES ig_object (object_id) ON DELETE CASCADE,
    ref_plane              VARCHAR(32),        -- Static/Fixed/Equator/Ecliptic/Laplace/Extrasolar
    position_x             DOUBLE PRECISION,   -- Position[0..2]，m
    position_y             DOUBLE PRECISION,
    position_z             DOUBLE PRECISION,
    period                 DOUBLE PRECISION,   -- s，逆行轨道为负
    pericenter_dist        DOUBLE PRECISION,   -- m
    aphelion_dist          DOUBLE PRECISION,   -- m
    semi_major_axis        DOUBLE PRECISION,   -- m
    eccentricity           DOUBLE PRECISION,
    inclination            DOUBLE PRECISION,   -- deg
    inclination_ecliptic   DOUBLE PRECISION,   -- deg，仅 --absolute-orbit 关闭时有值
    ascending_node         DOUBLE PRECISION,   -- deg
    asc_node_ecliptic      DOUBLE PRECISION,   -- deg
    epoch                  DOUBLE PRECISION,   -- JD
    arg_of_pericenter      DOUBLE PRECISION,   -- deg
    arg_of_peri_ecliptic   DOUBLE PRECISION,   -- deg
    mean_anomaly           DOUBLE PRECISION,   -- deg
    synodic_month          DOUBLE PRECISION,   -- s，仅卫星/双星系伴星
    binary_orbit           BOOLEAN NOT NULL DEFAULT FALSE,
    is_primary             BOOLEAN,
    primary_name           VARCHAR(255),
    companion_name         VARCHAR(255),
    b_period               DOUBLE PRECISION,
    b_pericenter_dist      DOUBLE PRECISION,
    b_aphelion_dist        DOUBLE PRECISION,
    b_semi_major_axis      DOUBLE PRECISION,
    b_eccentricity         DOUBLE PRECISION,
    b_inclination          DOUBLE PRECISION,
    b_inclination_ecliptic DOUBLE PRECISION,
    b_ascending_node       DOUBLE PRECISION,
    b_asc_node_ecliptic    DOUBLE PRECISION,
    b_epoch                DOUBLE PRECISION,
    b_arg_of_pericenter    DOUBLE PRECISION,
    b_arg_of_peri_ecliptic DOUBLE PRECISION,
    b_mean_anomaly         DOUBLE PRECISION
);
-- CREATE INDEX ix_ig_orbit_sma  ON ig_orbit (semi_major_axis);
-- CREATE INDEX ix_ig_orbit_incl ON ig_orbit (inclination);
-- CREATE INDEX ix_ig_orbit_per  ON ig_orbit (period);

CREATE TABLE ig_atmosphere (
    object_id        VARCHAR(512) PRIMARY KEY REFERENCES ig_object (object_id) ON DELETE CASCADE,
    surface_pressure DOUBLE PRECISION,  -- Pa
    scale_height     DOUBLE PRECISION   -- m
);

CREATE TABLE ig_hydrosphere (
    object_id VARCHAR(512) PRIMARY KEY REFERENCES ig_object (object_id) ON DELETE CASCADE,
    height    DOUBLE PRECISION          -- m
);

CREATE TABLE ig_biosphere (
    object_id VARCHAR(512) PRIMARY KEY REFERENCES ig_object (object_id) ON DELETE CASCADE,
    bio_class VARCHAR(64),              -- Biosphere.Class
    bio_type  VARCHAR(64)               -- Biosphere.Type
);

CREATE TABLE ig_biosphere_biome (
    object_id VARCHAR(512) NOT NULL REFERENCES ig_object (object_id) ON DELETE CASCADE,
    biome     VARCHAR(64)  NOT NULL,
    PRIMARY KEY (object_id, biome)
);

-- 大气与海洋的成分结构完全一致，合成一张表用kind区分
CREATE TABLE ig_composition (
    object_id VARCHAR(512) NOT NULL REFERENCES ig_object (object_id) ON DELETE CASCADE,
    kind      VARCHAR(16)  NOT NULL,     -- 'atmosphere'|'hydrosphere'
    component VARCHAR(64)  NOT NULL,
    fraction  DOUBLE PRECISION,         -- 体积百分比，CompositionByVolume
    PRIMARY KEY (object_id, kind, component)
);
-- CREATE INDEX ix_ig_composition_lookup ON ig_composition (kind, component);

COMMIT;