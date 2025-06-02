"use client"

import { TrendingUp } from "lucide-react"
import * as XLSX from 'xlsx'
import { CartesianGrid, Line, LineChart, ResponsiveContainer, XAxis, YAxis } from "recharts"

import {
    Card,
    CardContent,
    CardDescription,
    CardFooter,
    CardHeader,
    CardTitle,
} from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import {
    ChartConfig,
    ChartContainer,
    ChartTooltip,
    ChartTooltipContent,
} from "@/components/ui/chart"
import { useEffect, useState } from "react"
import { getHistoricalDataTb } from "@/actions"
import { formatDate } from "@/lib/utils"
import { CustomTooltip } from "./CustomTooltip"
import { Button } from "../ui/button"

type SensorData = {
    time: string,
    value: number
}

const HISTORICAL_RANGES = {
    '90d': { days: 90, interval: 5 * 60 * 60 * 1000, description: 'Last 3 months' },
    '30d': { days: 30, interval: 2 * 60 * 60 * 1000, description: 'Last 30 days' },
    '7d': { days: 7, interval: 30 * 60 * 1000, description: 'Last 7 days' },
    'current': { days: 0, interval: 30 * 60 * 1000, description: 'Current time' }
}

// const chartData = [
//     { month: "January", desktop: 186, mobile: 80 },
//     { month: "February", desktop: 305, mobile: 200 },
//     { month: "March", desktop: 237, mobile: 120 },
//     { month: "April", desktop: 73, mobile: 190 },
//     { month: "May", desktop: 209, mobile: 130 },
//     { month: "June", desktop: 214, mobile: 140 },
// ]

const chartConfig = {
    time: {
        label: "Time E"
    },
    desktop: {
        label: "Desktop",
        color: "hsl(var(--chart-1))",
    },
    mobile: {
        label: "Mobile",
        color: "hsl(var(--chart-2))",
    },
} satisfies ChartConfig

export function LineChartData({ realData, predictData, title, deviceId }: { realData: SensorData[], predictData: SensorData[], title: string, deviceId: string }) {

    const [timeRange, setTimeRange] = useState<'90d' | '30d' | '7d' | 'current'>('current')

    const [historicalReal, setHistoricalReal] = useState<SensorData[]>([])
    const [liveRealData, setLiveRealData] = useState<SensorData[]>(realData)

    const [historicalPredict, setHistoricalPredict] = useState<SensorData[]>([])
    const [livePredictData, setLivePredictData] = useState<SensorData[]>(predictData)

    useEffect(() => {
        let cancelled = false

        const getHistoricalData = async () => {
            const now = Date.now()
            const days = HISTORICAL_RANGES[timeRange].days
            const startTs = Date.now() - days * 24 * 60 * 60 * 1000
            const endTs = now
            const interval = HISTORICAL_RANGES[timeRange].interval
            const token = global?.window?.localStorage.getItem('token') ?? ''


            const resRealData = await getHistoricalDataTb(token, deviceId, title, startTs.toString(), endTs.toString(), interval.toString(), 'AVG', '100000')
            console.log('resRealData', resRealData)
            const resPredictData = await getHistoricalDataTb(token, deviceId, `predicted_${title}`, startTs.toString(), endTs.toString(), interval.toString(), 'AVG', '100000')
            console.log('resPredictData', resPredictData)
            const arrReal: SensorData[] = (resRealData[title] || []).map(({ ts, value }) => ({ time: ts, value }))
            const arrPredict: SensorData[] = (resPredictData[`predicted_${title}`] || []).map(({ ts, value }) => ({ time: ts, value }))

            if (!cancelled) {
                setHistoricalReal(arrReal)
                setLiveRealData([])
                setHistoricalPredict(arrPredict)
                setLivePredictData([])
            }
        }

        getHistoricalData()
        return () => {
            cancelled = true
        }
    }, [timeRange])

    useEffect(() => {
        setLiveRealData((prev) => {
            const lastTs = prev.length ? prev[prev.length - 1].time : historicalReal.length ? historicalReal[historicalReal.length - 1].time : 0

            const toAdd = realData.filter((pt) => pt.time > lastTs)
            return toAdd.length ? [...prev, ...toAdd] : prev
        })
        setLivePredictData((prev) => {
            const lastTs = prev.length ? prev[prev.length - 1].time : historicalPredict.length ? historicalPredict[historicalPredict.length - 1].time : 0

            const toAdd = predictData.filter((pt) => pt.time > lastTs)
            return toAdd.length ? [...prev, ...toAdd] : prev
        })
    }, [realData, historicalReal, predictData, historicalPredict])

    const mergedRealData = (timeRange === 'current') ? realData : [...historicalReal, ...liveRealData]
    const mergedPredictData = (timeRange === 'current') ? predictData : [...historicalPredict, ...livePredictData]

    const mergedData = mergedRealData.map((pt, i) => ({
        time: pt.time,
        real: pt.value,
        predicted: mergedPredictData[i]?.value ?? null,
    }))


    function exportFile(type: 'xlsx' | 'csv') {
        if (!mergedRealData.length) return

        // Build a worksheet: two columns "Time" & "Value"
        const sheet = XLSX.utils.json_to_sheet(
            mergedRealData.map((d) => ({
                Time: formatDate(true, d.time), // or however you want the text
                Value: d.value,
            }))
        )

        // Make a new workbook & append
        const wb = XLSX.utils.book_new()
        XLSX.utils.book_append_sheet(wb, sheet, 'Data')

        if (type === 'xlsx') {
            // This will prompt download of .xlsx
            XLSX.writeFile(wb, `${title}-data.xlsx`)
        } else {
            // Or convert to CSV + blob + a-tag download
            const csvBlob = new Blob(
                [XLSX.utils.sheet_to_csv(sheet)],
                { type: 'text/csv;charset=utf-8;' }
            )
            const url = URL.createObjectURL(csvBlob)
            const a = document.createElement('a')
            a.href = url
            a.download = `${title}-data.csv`
            a.click()
            URL.revokeObjectURL(url)
        }
    }

    return (
        <Card>
            <CardHeader>
                <CardTitle>Line Chart - {title}</CardTitle>
                <CardDescription>Data time range: {HISTORICAL_RANGES[timeRange].description}</CardDescription>
                <div className="flex gap-2 justify-between items-center">
                    <div className="flex gap-2">
                        <Button size="sm" onClick={() => exportFile('csv')}>
                            Export CSV
                        </Button>
                        <Button size="sm" onClick={() => exportFile('xlsx')}>
                            Export XLSX
                        </Button>
                    </div>
                    <Select value={timeRange} onValueChange={(v) => setTimeRange(v as '90d' | '30d' | '7d' | 'current')}>
                        <SelectTrigger className="w-[160px] rounded-lg sm:ml-auto" aria-label="Select a value">
                            <SelectValue placeholder="Current time" />
                        </SelectTrigger>
                        <SelectContent className="rounded-xl">
                            {Object.entries(HISTORICAL_RANGES).map(([key, { description }]) => (
                                <SelectItem key={key} value={key} className="rounded-lg">{description}</SelectItem>
                            ))}
                        </SelectContent>
                    </Select>
                </div>
            </CardHeader>
            <CardContent>
                <ChartContainer config={chartConfig} className="w-[100%] h-[50%]">
                    <LineChart
                        accessibilityLayer
                        data={mergedData.map((d) => ({
                            time: d.time,
                            real: d.real,
                            predicted: d.predicted
                        }))}
                        margin={{
                            left: -24,
                            right: -24,
                        }}
                    >
                        <CartesianGrid />
                        <XAxis
                            dataKey="time"
                            tick={{ fontSize: 12 }}
                            tickFormatter={(ts: number) => {

                                if (timeRange === 'current') return formatDate(false, ts)
                                const d = new Date(ts)
                                // "Jan 16" → "Jan-16"
                                return (
                                    d.toLocaleString('en-US', { month: 'short', day: '2-digit' })
                                )
                            }}
                            tickLine={true}
                            axisLine={true}
                            tickMargin={8}
                        />
                        <YAxis
                            dataKey="real"
                            tickLine={false}
                            axisLine={true}
                            tickMargin={8}
                        />
                        <ChartTooltip cursor={false} content={<CustomTooltip />} />
                        <Line
                            dataKey="real"
                            type="monotone"
                            stroke="var(--color-desktop)"
                            strokeWidth={2}
                            dot={false}
                        />
                        <Line
                            dataKey="predicted"
                            type="monotone"
                            stroke="var(--color-mobile)"
                            strokeWidth={2}
                            dot={false}
                        />
                    </LineChart>
                </ChartContainer>
            </CardContent>
            <CardFooter>
                {/* <div className="flex w-full items-start gap-2 text-sm">
                    <div className="grid gap-2">
                        <div className="flex items-center gap-2 font-medium leading-none">
                            Trending up by 5.2% this month <TrendingUp className="h-4 w-4" />
                        </div>
                        <div className="flex items-center gap-2 leading-none text-muted-foreground">
                            Showing total visitors for the last 6 months
                        </div>
                    </div>
                </div> */}
            </CardFooter>
        </Card>
    )
}
