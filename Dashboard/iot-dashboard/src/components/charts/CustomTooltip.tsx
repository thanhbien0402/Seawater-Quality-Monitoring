'use client'

import { formatDate } from '@/lib/utils'
import React, { useEffect } from 'react'
import { TooltipProps } from 'recharts'

interface PayloadItem {
    dataKey: string
    value: number
    color: string
    name?: string
}

export function CustomTooltip({
    active,
    payload,
    label,
}: TooltipProps<string, number>) {
    if (!active || !payload || !payload.length) {
        return null
    }

    useEffect(() => {
        console.log('label >>>', label, typeof (label))
    })

    // payload is an array of series points; each has value, color, dataKey, etc.
    // label is your X‐axis value (timestamp string)
    return (
        <div className="bg-white p-2 rounded-lg shadow-md text-sm" >
            {/* 1) Top line: the date/time */}
            <div className="mb-2 border-b pb-1 font-medium">
                {formatDate(true, label)}
            </div>

            {/* 2) One line per series: colored bullet + name + value */}
            {payload.map((entry: any) => (
                <div key={entry.dataKey} className="flex items-center gap-2">
                    {/* colored dot */}
                    <span
                        className="inline-block w-2 h-2 rounded-full"
                        style={{ backgroundColor: entry.color }}
                    />
                    {/* series name (fallback to dataKey) */}
                    <span className="capitalize">{entry.name ?? entry.dataKey}</span>:
                    {/* the value */}
                    <span className="font-semibold">{Number(entry.value).toFixed(2)}</span>
                </div>
            ))}
        </div>
    )
}
