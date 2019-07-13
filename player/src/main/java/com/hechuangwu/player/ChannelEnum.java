package com.hechuangwu.player;

/**
 * Created by cwh on 2019/7/13 0013.
 * 功能:
 */
public enum ChannelEnum {
    LEFT( 0 ), RIGHT( 1 ), CENTER( 2 );

    private int type;

    ChannelEnum(int type) {
        this.type = type;
    }

    public int getType() {
        return type;
    }
}
